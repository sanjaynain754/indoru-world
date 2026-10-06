import { Engine } from "@babylonjs/core/Engines/engine";
import { Scene } from "@babylonjs/core/scene";
import { FreeCamera } from "@babylonjs/core/Cameras/freeCamera";
import { HemisphericLight } from "@babylonjs/core/Lights/hemisphericLight";
import { DirectionalLight } from "@babylonjs/core/Lights/directionalLight";
import { ShadowGenerator } from "@babylonjs/core/Lights/Shadows/shadowGenerator";
import { StandardMaterial } from "@babylonjs/core/Materials/standardMaterial";
import { Texture } from "@babylonjs/core/Materials/Textures/texture";
import { Color3, Color4 } from "@babylonjs/core/Maths/math.color";
import { Vector3 } from "@babylonjs/core/Maths/math.vector";
import { MeshBuilder } from "@babylonjs/core/Meshes/meshBuilder";
import type { AbstractMesh } from "@babylonjs/core/Meshes/abstractMesh";
import { DefaultRenderingPipeline } from "@babylonjs/core/PostProcesses/RenderPipeline/Pipelines/defaultRenderingPipeline";
import type { Material } from "@babylonjs/core/Materials/material";
import { InputManager } from "./input/InputManager";
import { GameWorld } from "./world/GameWorld";
import { HudController } from "./ui/HudController";
import { CountryTerrain } from "./world/CountryTerrain";
import { AdvancedWaterMaterial, registerAdvancedWaterShader } from "./rendering/AdvancedWaterMaterial";
import { pbrMaterial } from "./rendering/PbrMaterials";

const SKYLINE_URL = import.meta.env.VITE_SKYLINE_URL ?? `${import.meta.env.BASE_URL}assets/navaar-skyline.png`;

export interface GameHandle { scene: Scene; dispose: () => void; }

function material(scene: Scene, name: string, color: string, emissive = "#000000"): StandardMaterial {
  const result = new StandardMaterial(name, scene);
  result.diffuseColor = Color3.FromHexString(color);
  result.specularColor = Color3.FromHexString("#273849");
  if (emissive !== "#000000") result.emissiveColor = Color3.FromHexString(emissive);
  return result;
}

function box(scene: Scene, name: string, size: { width: number; height: number; depth: number }, position: Vector3, mat: Material): AbstractMesh {
  const mesh = MeshBuilder.CreateBox(name, size, scene);
  mesh.position.copyFrom(position);
  mesh.material = mat;
  return mesh;
}

function buildDistrict(scene: Scene): void {
  const asphalt = material(scene, "asphalt", "#172735");
  const roadMark = material(scene, "road-mark", "#D6B665", "#6B5B2B");
  const sidewalk = material(scene, "sidewalk", "#314451");
  registerAdvancedWaterShader();
  const water = new AdvancedWaterMaterial(scene, "navaar-river-water");
  const sandstone = material(scene, "sandstone", "#7B675B");
  const glass = material(scene, "glass", "#2A7B85", "#103A4A");
  const amber = material(scene, "windows", "#E9B949", "#9F6B18");
  const rail = material(scene, "rail", "#536C79");
  const ground = MeshBuilder.CreateGround("district-ground", { width: 120, height: 120 }, scene);
  ground.material = pbrMaterial(scene, "navaar-ground-pbr", "#243e43", { roughness: 0.92, metallic: 0.01, environmentIntensity: 0.8 });
  box(scene, "river", { width: 120, height: 0.08, depth: 11 }, new Vector3(0, 0.02, -50), water);
  for (const z of [-21, 0, 21]) {
    box(scene, `road-h-${z}`, { width: 100, height: 0.12, depth: 9.5 }, new Vector3(0, 0.06, z), asphalt);
    for (let x = -44; x <= 44; x += 8) box(scene, `mark-h-${z}-${x}`, { width: 4, height: 0.03, depth: 0.12 }, new Vector3(x, 0.14, z), roadMark);
  }
  for (const x of [-28, 28]) {
    box(scene, `road-v-${x}`, { width: 9.5, height: 0.12, depth: 110 }, new Vector3(x, 0.06, 0), asphalt);
    for (let z = -50; z <= 50; z += 8) box(scene, `mark-v-${x}-${z}`, { width: 0.12, height: 0.03, depth: 4 }, new Vector3(x, 0.14, z), roadMark);
  }
  box(scene, "quay-walk", { width: 100, height: 0.18, depth: 5 }, new Vector3(0, 0.15, -42), sidewalk);
  box(scene, "rail-viaduct", { width: 96, height: 0.7, depth: 3 }, new Vector3(0, 7, 29), rail);
  for (const x of [-40, -20, 0, 20, 40]) {
    box(scene, `rail-pillar-a-${x}`, { width: 0.9, height: 7, depth: 0.9 }, new Vector3(x, 3.5, 28), rail);
    box(scene, `rail-pillar-b-${x}`, { width: 0.9, height: 7, depth: 0.9 }, new Vector3(x, 3.5, 30), rail);
  }
  const lots = [
    [-43, -34, 10, 13, 8], [-31, -34, 8, 18, 8], [-17, -34, 11, 11, 8], [17, -34, 11, 18, 8], [32, -34, 9, 13, 8], [44, -34, 8, 22, 8],
    [-43, -10, 10, 15, 9], [-17, -10, 11, 25, 9], [17, -10, 11, 14, 9], [43, -10, 10, 20, 9],
    [-43, 12, 10, 22, 9], [-17, 12, 11, 14, 9], [17, 12, 11, 26, 9], [43, 12, 10, 16, 9],
    [-43, 34, 10, 18, 8], [-31, 34, 8, 12, 8], [-17, 34, 11, 22, 8], [17, 34, 11, 12, 8], [32, 34, 9, 24, 8], [44, 34, 8, 15, 8],
  ];
  lots.forEach(([x, z, width, height, depth], index) => {
    const mat = index % 3 === 0 ? glass : sandstone;
    box(scene, `building-${index}`, { width, height, depth }, new Vector3(x, height / 2, z), mat);
    if (index % 2 === 0) {
      for (let level = 1; level < Math.min(5, Math.floor(height / 4)); level += 1) {
        box(scene, `window-${index}-${level}`, { width: Math.max(1, width * 0.5), height: 0.15, depth: 0.05 }, new Vector3(x, level * 3.4, z - depth / 2 - 0.04), amber);
      }
    }
  });
  const lampMat = material(scene, "lamp", "#D5B15C", "#F4C65B");
  for (const x of [-44, -22, 0, 22, 44]) {
    box(scene, `lamp-${x}`, { width: 0.15, height: 3.4, depth: 0.15 }, new Vector3(x, 1.7, -38), lampMat);
    box(scene, `lamp-head-${x}`, { width: 0.55, height: 0.12, depth: 0.55 }, new Vector3(x, 3.45, -38), lampMat);
  }
  const beaconMat = material(scene, "checkpoint", "#F5B94D", "#F5B94D");
  const beacon = MeshBuilder.CreateCylinder("mission-beacon", { height: 2.8, diameter: 0.35, tessellation: 12 }, scene);
  beacon.position.set(0, 1.4, -43);
  beacon.material = beaconMat;
  const ring = MeshBuilder.CreateTorus("mission-ring", { diameter: 5, thickness: 0.12, tessellation: 32 }, scene);
  ring.position.set(0, 0.18, -43);
  ring.rotation.x = Math.PI / 2;
  ring.material = beaconMat;
}

export async function createGameScene(engine: Engine, canvas: HTMLCanvasElement): Promise<GameHandle> {
  const scene = new Scene(engine);
  scene.clearColor = new Color4(0.025, 0.055, 0.08, 1);
  scene.fogMode = Scene.FOGMODE_EXP2;
  scene.fogDensity = 0.006;
  scene.fogColor = new Color3(0.025, 0.055, 0.08);
  buildDistrict(scene);
  const skyMat = new StandardMaterial("skyline-backdrop", scene);
  skyMat.diffuseTexture = new Texture(SKYLINE_URL, scene);
  skyMat.emissiveTexture = skyMat.diffuseTexture;
  skyMat.backFaceCulling = false;
  const skyline = MeshBuilder.CreatePlane("skyline-backdrop", { width: 125, height: 70 }, scene);
  skyline.position.set(0, 28, 72);
  skyline.material = skyMat;
  const hemi = new HemisphericLight("navaar-sky", new Vector3(0.2, 1, 0.1), scene);
  hemi.intensity = 0.75;
  hemi.diffuse = Color3.FromHexString("#C7D7E6");
  hemi.groundColor = Color3.FromHexString("#152431");
  const sun = new DirectionalLight("late-sun", new Vector3(-0.4, -1, 0.5), scene);
  sun.position = new Vector3(-40, 70, -30);
  sun.intensity = 1.1;
  sun.diffuse = Color3.FromHexString("#FFD6A1");
  const shadows = new ShadowGenerator(1024, sun);
  shadows.useBlurExponentialShadowMap = true;
  shadows.blurKernel = 24;
  const camera = new FreeCamera("chase-camera", new Vector3(0, 7, 23), scene);
  camera.fov = 0.92;
  camera.minZ = 0.1;
  camera.maxZ = 300;
  camera.attachControl(canvas, false);
  camera.inputs.clear();
  const pipeline = new DefaultRenderingPipeline("indoru-hdr-pipeline", true, scene, [camera]);
  pipeline.fxaaEnabled = true;
  pipeline.samples = 4;
  pipeline.bloomEnabled = true;
  pipeline.bloomThreshold = 0.78;
  pipeline.bloomWeight = 0.16;
  pipeline.bloomKernel = 64;
  pipeline.imageProcessingEnabled = true;
  pipeline.imageProcessing.contrast = 1.18;
  pipeline.imageProcessing.exposure = 1.08;
  pipeline.imageProcessing.vignetteEnabled = true;
  pipeline.imageProcessing.vignetteWeight = 1.25;
  pipeline.imageProcessing.vignetteStretch = 0.35;
  pipeline.imageProcessing.vignetteColor = new Color4(0.027, 0.075, 0.114, 1);
  const input = new InputManager();
  const world = new GameWorld(scene, input);
  scene.meshes.forEach((mesh) => {
    mesh.receiveShadows = true;
    if (mesh.name !== "district-ground" && mesh.name !== "skyline-backdrop") shadows.addShadowCaster(mesh, true);
  });
  const countryTerrain = new CountryTerrain(scene);
  const hud = new HudController(async (country) => {
    await countryTerrain.load(country);
  });
  const demo = new URLSearchParams(window.location.search).has("demo");
  let cameraPosition = camera.position.clone();
  const observer = scene.onBeforeRenderObservable.add(() => {
    const delta = Math.min(0.05, engine.getDeltaTime() / 1000);
    world.update(delta, demo);
    const heading = world.player.heading;
    const desired = world.player.root.position.add(new Vector3(-Math.sin(heading) * 13, 7.6, -Math.cos(heading) * 13));
    cameraPosition = Vector3.Lerp(cameraPosition, desired, Math.min(1, delta * 4.5));
    camera.position.copyFrom(cameraPosition);
    camera.setTarget(world.player.root.position.add(new Vector3(Math.sin(heading) * 8, 1.15, Math.cos(heading) * 8)));
    hud.update(world.snapshot());
  });
  return {
    scene,
    dispose: () => {
      scene.onBeforeRenderObservable.remove(observer);
      hud.dispose();
      countryTerrain.dispose();
      input.dispose();
      world.dispose();
      scene.dispose();
    },
  };
}
