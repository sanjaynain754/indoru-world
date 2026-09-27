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
import { InputManager } from "./input/InputManager";
import { GameWorld } from "./world/GameWorld";
import { HudController } from "./ui/HudController";

const SKYLINE_URL = import.meta.env.VITE_SKYLINE_URL ?? `${import.meta.env.BASE_URL}assets/navaar-skyline.png`;

export interface GameHandle { scene: Scene; dispose: () => void; }

function material(scene: Scene, name: string, color: string, emissive = "#000000"): StandardMaterial {
  const result = new StandardMaterial(name, scene);
  result.diffuseColor = Color3.FromHexString(color);
  result.specularColor = Color3.FromHexString("#273849");
  if (emissive !== "#000000") result.emissiveColor = Color3.FromHexString(emissive);
  return result;
}

function box(scene: Scene, name: string, size: { width: number; height: number; depth: number }, position: Vector3, mat: StandardMaterial): AbstractMesh {
  const mesh = MeshBuilder.CreateBox(name, size, scene);
  mesh.position.copyFrom(position);
  mesh.material = mat;
  return mesh;
}

function buildDistrict(scene: Scene): void {
  const asphalt = material(scene, "asphalt", "#172735");
  const roadMark = material(scene, "road-mark", "#D6B665", "#6B5B2B");
  const sidewalk = material(scene, "sidewalk", "#314451");
  const water = material(scene, "river", "#0B5261", "#06313C");
  const sandstone = material(scene, "sandstone", "#7B675B");
  const glass = material(scene, "glass", "#2A7B85", "#103A4A");
  const amber = material(scene, "windows", "#E9B949", "#9F6B18");
  const facadeGlass = material(scene, "facade-glass", "#76AEB0", "#31565A");
  const rail = material(scene, "rail", "#536C79");
  const ground = MeshBuilder.CreateGround("district-ground", { width: 120, height: 120 }, scene);
  ground.material = material(scene, "ground", "#0D1C26");
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
    for (let level = 2.8; level < height - 1.2; level += 3.4) {
      for (let offset = -width / 2 + 1.5; offset < width / 2 - 0.5; offset += 2.8) {
        const lit = (index + Math.round(level + offset)) % 4 === 0;
        for (const side of [-1, 1]) {
          box(scene, `facade-window-${index}-${level}-${offset}-${side}`, { width: 0.78, height: 0.92, depth: 0.06 }, new Vector3(x + offset, level, z + side * (depth / 2 + 0.04)), lit ? amber : facadeGlass);
        }
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

function buildWaterfront(scene: Scene): void {
  const stone = material(scene, "quay-stone", "#647681");
  const timber = material(scene, "pier-timber", "#765D49");
  const foliage = material(scene, "quay-foliage", "#28665E", "#0D302C");
  const trunk = material(scene, "quay-tree-trunk", "#705746");
  const warmLight = material(scene, "quay-lantern", "#F2C977", "#F6B948");

  for (let x = -48; x <= 48; x += 8) {
    box(scene, `quay-post-${x}`, { width: 0.16, height: 0.8, depth: 0.16 }, new Vector3(x, 0.55, -44.2), stone);
  }
  box(scene, "quay-handrail", { width: 97, height: 0.16, depth: 0.16 }, new Vector3(0, 0.91, -44.2), stone);

  for (const x of [-39, -27, -15, 15, 27, 39]) {
    const bench = box(scene, `quay-bench-seat-${x}`, { width: 2.5, height: 0.16, depth: 0.72 }, new Vector3(x, 0.65, -39.1), timber);
    bench.rotation.y = Math.PI / 2;
    const back = box(scene, `quay-bench-back-${x}`, { width: 2.5, height: 0.8, depth: 0.12 }, new Vector3(x, 1.08, -38.72), timber);
    back.rotation.y = Math.PI / 2;
    for (const legX of [x - 0.75, x + 0.75]) {
      box(scene, `quay-bench-leg-${x}-${legX}`, { width: 0.12, height: 0.62, depth: 0.12 }, new Vector3(legX, 0.32, -39.1), stone);
    }
  }

  for (const x of [-42, 42]) {
    const stem = MeshBuilder.CreateCylinder(`quay-lamp-stem-${x}`, { height: 4.8, diameter: 0.16, tessellation: 8 }, scene);
    stem.position.set(x, 2.4, -36.8);
    stem.material = stone;
    const head = MeshBuilder.CreateSphere(`quay-lamp-glow-${x}`, { diameter: 0.72, segments: 10 }, scene);
    head.position.set(x, 4.8, -36.8);
    head.material = warmLight;
  }

  const pier = box(scene, "river-pier-deck", { width: 13, height: 0.32, depth: 4 }, new Vector3(29, 0.3, -46.7), timber);
  pier.rotation.y = -0.06;
  for (const x of [24, 34]) {
    for (const z of [-48, -45]) {
      const pile = MeshBuilder.CreateCylinder(`pier-pile-${x}-${z}`, { height: 1.4, diameter: 0.24, tessellation: 8 }, scene);
      pile.position.set(x, -0.35, z);
      pile.material = timber;
    }
  }
  box(scene, "harbor-launch-hull", { width: 8.5, height: 0.62, depth: 2.6 }, new Vector3(-24, 0.42, -50.5), material(scene, "launch-hull", "#C77A50"));
  box(scene, "harbor-launch-cabin", { width: 3.2, height: 1.1, depth: 1.8 }, new Vector3(-24, 1.15, -50.5), material(scene, "launch-cabin", "#E1D5B9"));

  for (const x of [-36, 36]) {
    const treeTrunk = MeshBuilder.CreateCylinder(`quay-tree-trunk-${x}`, { height: 2.6, diameter: 0.34, tessellation: 8 }, scene);
    treeTrunk.position.set(x, 1.3, -35.4);
    treeTrunk.material = trunk;
    const crown = MeshBuilder.CreateSphere(`quay-tree-crown-${x}`, { diameter: 3.6, segments: 8 }, scene);
    crown.position.set(x, 3.1, -35.4);
    crown.material = foliage;
  }
}

export async function createGameScene(engine: Engine, canvas: HTMLCanvasElement): Promise<GameHandle> {
  const scene = new Scene(engine);
  scene.clearColor = new Color4(0.025, 0.055, 0.08, 1);
  scene.fogMode = Scene.FOGMODE_EXP2;
  scene.fogDensity = 0.006;
  scene.fogColor = new Color3(0.025, 0.055, 0.08);
  buildDistrict(scene);
  buildWaterfront(scene);
  const skyMat = new StandardMaterial("skyline-backdrop", scene);
  skyMat.diffuseTexture = new Texture(SKYLINE_URL, scene);
  skyMat.emissiveTexture = skyMat.diffuseTexture;
  skyMat.backFaceCulling = false;
  const skyline = MeshBuilder.CreatePlane("skyline-backdrop", { width: 125, height: 70 }, scene);
  skyline.position.set(0, 28, 72);
  skyline.material = skyMat;
  const hemi = new HemisphericLight("navaar-sky", new Vector3(0.2, 1, 0.1), scene);
  hemi.intensity = 1.05;
  hemi.diffuse = Color3.FromHexString("#C7D7E6");
  hemi.groundColor = Color3.FromHexString("#152431");
  const sun = new DirectionalLight("late-sun", new Vector3(-0.4, -1, 0.5), scene);
  sun.position = new Vector3(-40, 70, -30);
  sun.intensity = 1.28;
  sun.diffuse = Color3.FromHexString("#FFD6A1");
  const shadows = new ShadowGenerator(1024, sun);
  shadows.useBlurExponentialShadowMap = true;
  shadows.blurKernel = 24;
  shadows.setDarkness(0.48);
  const camera = new FreeCamera("chase-camera", new Vector3(0, 7, 23), scene);
  camera.fov = 0.92;
  camera.minZ = 0.1;
  camera.maxZ = 300;
  camera.attachControl(canvas, false);
  camera.inputs.clear();
  const input = new InputManager();
  const world = new GameWorld(scene, input);
  scene.meshes.forEach((mesh) => {
    mesh.receiveShadows = true;
    if (mesh.name !== "district-ground" && mesh.name !== "skyline-backdrop" && !mesh.name.startsWith("facade-window-")) shadows.addShadowCaster(mesh, true);
  });
  const hud = new HudController(input);
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
      input.dispose();
      world.dispose();
      scene.dispose();
    },
  };
}
