import { MeshBuilder } from "@babylonjs/core/Meshes/meshBuilder";
import { StandardMaterial } from "@babylonjs/core/Materials/standardMaterial";
import { Color3 } from "@babylonjs/core/Maths/math.color";
import { Vector3 } from "@babylonjs/core/Maths/math.vector";
import { TransformNode } from "@babylonjs/core/Meshes/transformNode";
import type { AbstractMesh } from "@babylonjs/core/Meshes/abstractMesh";
import type { Scene } from "@babylonjs/core/scene";
import type { InputManager } from "../input/InputManager";

const clamp = (value: number, min: number, max: number) => Math.max(min, Math.min(max, value));

function boxPart(scene: Scene, root: TransformNode, name: string, size: { width: number; height: number; depth: number }, position: Vector3, material: StandardMaterial): AbstractMesh {
  const mesh = MeshBuilder.CreateBox(name, size, scene);
  mesh.parent = root;
  mesh.position.copyFrom(position);
  mesh.material = material;
  return mesh;
}

export class PlayerVehicle {
  readonly root: TransformNode;
  speed = 0;
  heading = 0;
  health = 100;
  private readonly wheels: AbstractMesh[] = [];

  constructor(private readonly scene: Scene, start: Vector3) {
    this.root = new TransformNode("player-sedan", scene);
    this.root.position.copyFrom(start);

    const bodyMat = new StandardMaterial("player-teal", scene);
    bodyMat.diffuseColor = Color3.FromHexString("#12B8B0");
    bodyMat.specularColor = Color3.FromHexString("#A7F5EA");
    const darkMat = new StandardMaterial("player-trim", scene);
    darkMat.diffuseColor = Color3.FromHexString("#102433");
    darkMat.specularColor = Color3.FromHexString("#718B96");
    const glassMat = new StandardMaterial("player-glass", scene);
    glassMat.diffuseColor = Color3.FromHexString("#387D88");
    glassMat.specularColor = Color3.FromHexString("#C4F4ED");
    glassMat.alpha = 0.82;
    const chromeMat = new StandardMaterial("player-chrome", scene);
    chromeMat.diffuseColor = Color3.FromHexString("#A6B8B9");
    chromeMat.specularColor = Color3.FromHexString("#F3FFFF");
    const tireMat = new StandardMaterial("player-tires", scene);
    tireMat.diffuseColor = Color3.FromHexString("#111820");
    const headlightMat = new StandardMaterial("player-headlights", scene);
    headlightMat.diffuseColor = Color3.FromHexString("#FFF1C7");
    headlightMat.emissiveColor = Color3.FromHexString("#FFD987");
    const tailLightMat = new StandardMaterial("player-tail-lights", scene);
    tailLightMat.diffuseColor = Color3.FromHexString("#F15E53");
    tailLightMat.emissiveColor = Color3.FromHexString("#7B1D25");

    boxPart(scene, this.root, "player-body", { width: 2.25, height: 0.62, depth: 4.3 }, new Vector3(0, 0.62, 0), bodyMat);
    boxPart(scene, this.root, "player-cabin", { width: 1.72, height: 0.58, depth: 1.82 }, new Vector3(0, 1.13, -0.14), glassMat);
    boxPart(scene, this.root, "player-roof", { width: 1.52, height: 0.14, depth: 1.18 }, new Vector3(0, 1.48, -0.18), bodyMat);
    boxPart(scene, this.root, "player-hood", { width: 1.94, height: 0.12, depth: 1.02 }, new Vector3(0, 0.97, 1.35), bodyMat);
    boxPart(scene, this.root, "player-grille", { width: 0.92, height: 0.2, depth: 0.08 }, new Vector3(0, 0.69, 2.18), darkMat);
    boxPart(scene, this.root, "player-front-lip", { width: 1.86, height: 0.12, depth: 0.14 }, new Vector3(0, 0.36, 2.12), darkMat);
    boxPart(scene, this.root, "player-rear-bumper", { width: 1.9, height: 0.15, depth: 0.14 }, new Vector3(0, 0.4, -2.14), chromeMat);
    boxPart(scene, this.root, "player-number-plate", { width: 0.56, height: 0.16, depth: 0.04 }, new Vector3(0, 0.62, 2.23), chromeMat);

    for (const x of [-0.84, 0.84]) {
      boxPart(scene, this.root, `player-headlamp-${x}`, { width: 0.42, height: 0.18, depth: 0.08 }, new Vector3(x, 0.82, 2.19), headlightMat);
      boxPart(scene, this.root, `player-tail-lamp-${x}`, { width: 0.34, height: 0.18, depth: 0.08 }, new Vector3(x, 0.82, -2.19), tailLightMat);
      boxPart(scene, this.root, `player-mirror-${x}`, { width: 0.16, height: 0.15, depth: 0.3 }, new Vector3(x * 1.2, 1.02, 0.78), bodyMat);
      boxPart(scene, this.root, `player-door-trim-${x}`, { width: 0.06, height: 0.18, depth: 1.1 }, new Vector3(x * 1.03, 0.62, -0.18), chromeMat);
      boxPart(scene, this.root, `player-side-glass-${x}`, { width: 0.055, height: 0.33, depth: 1.25 }, new Vector3(x * 1.015, 1.16, -0.15), glassMat);
    }

    for (const x of [-1.05, 1.05]) {
      for (const z of [-1.35, 1.35]) {
        const wheel = MeshBuilder.CreateCylinder(`player-wheel-${x}-${z}`, { diameter: 0.56, height: 0.24, tessellation: 20 }, scene);
        wheel.parent = this.root;
        wheel.position.set(x, 0.35, z);
        wheel.rotation.z = Math.PI / 2;
        wheel.material = tireMat;
        this.wheels.push(wheel);
        const hub = MeshBuilder.CreateCylinder(`player-wheel-hub-${x}-${z}`, { diameter: 0.28, height: 0.255, tessellation: 12 }, scene);
        hub.parent = this.root;
        hub.position.set(x * 1.01, 0.35, z);
        hub.rotation.z = Math.PI / 2;
        hub.material = chromeMat;
      }
    }
    boxPart(scene, this.root, "player-beacon", { width: 0.26, height: 0.08, depth: 0.26 }, new Vector3(0, 1.6, -0.2), headlightMat);
  }

  update(delta: number, input: InputManager, demo = false): void {
    const forward = demo ? 0.42 : (input.isHeld("forward") ? 1 : 0) - (input.isHeld("back") ? 1 : 0);
    const steering = demo ? Math.sin(performance.now() / 2600) * 0.72 : (input.isHeld("right") ? 1 : 0) - (input.isHeld("left") ? 1 : 0);
    const braking = input.isHeld("brake");
    const acceleration = forward >= 0 ? 26 : 15;
    this.speed += forward * acceleration * delta;
    if (braking) this.speed *= Math.max(0, 1 - delta * 5.5);
    this.speed *= Math.max(0, 1 - delta * (Math.abs(forward) > 0 ? 0.42 : 1.4));
    this.speed = clamp(this.speed, -12, 32);
    const turnGrip = Math.min(1, Math.abs(this.speed) / 8);
    this.heading += steering * (0.8 + turnGrip * 0.9) * delta * (this.speed >= 0 ? 1 : -1);
    this.root.position.x += Math.sin(this.heading) * this.speed * delta;
    this.root.position.z += Math.cos(this.heading) * this.speed * delta;
    this.root.position.x = clamp(this.root.position.x, -46, 46);
    this.root.position.z = clamp(this.root.position.z, -54, 54);
    this.root.rotation.y = this.heading;
    this.root.rotation.z = -steering * turnGrip * 0.04;
    const wheelSpin = this.speed * delta * 1.7;
    this.wheels.forEach((wheel) => { wheel.rotation.x += wheelSpin; });
  }

  reset(start: Vector3): void {
    this.root.position.copyFrom(start);
    this.speed = 0;
    this.heading = 0;
    this.health = 100;
    this.root.rotation.set(0, 0, 0);
  }

  dispose(): void {
    this.root.dispose(false, true);
  }
}
