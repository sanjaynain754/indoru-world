import { MeshBuilder } from "@babylonjs/core/Meshes/meshBuilder";
import { StandardMaterial } from "@babylonjs/core/Materials/standardMaterial";
import { Color3 } from "@babylonjs/core/Maths/math.color";
import { Vector3 } from "@babylonjs/core/Maths/math.vector";
import { TransformNode } from "@babylonjs/core/Meshes/transformNode";
import type { Scene } from "@babylonjs/core/scene";
import type { InputManager } from "../input/InputManager";

const clamp = (value: number, min: number, max: number) => Math.max(min, Math.min(max, value));

export class PlayerVehicle {
  readonly root: TransformNode;
  speed = 0;
  heading = 0;
  health = 100;

  constructor(private readonly scene: Scene, start: Vector3) {
    this.root = new TransformNode("player-sedan", scene);
    this.root.position.copyFrom(start);
    const bodyMat = new StandardMaterial("player-teal", scene);
    bodyMat.diffuseColor = Color3.FromHexString("#12B8B0");
    bodyMat.specularColor = Color3.FromHexString("#8EE8E1");
    const darkMat = new StandardMaterial("player-dark", scene);
    darkMat.diffuseColor = Color3.FromHexString("#122337");
    const body = MeshBuilder.CreateBox("player-body", { width: 2.25, height: 0.62, depth: 4.3 }, scene);
    body.parent = this.root;
    body.position.y = 0.62;
    body.material = bodyMat;
    const cabin = MeshBuilder.CreateBox("player-cabin", { width: 1.75, height: 0.58, depth: 1.8 }, scene);
    cabin.parent = this.root;
    cabin.position.set(0, 1.12, -0.15);
    cabin.material = darkMat;
    for (const x of [-1.05, 1.05]) {
      for (const z of [-1.35, 1.35]) {
        const wheel = MeshBuilder.CreateCylinder(`player-wheel-${x}-${z}`, { diameter: 0.52, height: 0.22, tessellation: 16 }, scene);
        wheel.parent = this.root;
        wheel.position.set(x, 0.35, z);
        wheel.rotation.z = Math.PI / 2;
        wheel.material = darkMat;
      }
    }
    const beacon = MeshBuilder.CreateBox("player-beacon", { width: 0.28, height: 0.08, depth: 0.28 }, scene);
    beacon.parent = this.root;
    beacon.position.y = 1.52;
    beacon.material = bodyMat;
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
