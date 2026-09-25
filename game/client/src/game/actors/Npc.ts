import { MeshBuilder } from "@babylonjs/core/Meshes/meshBuilder";
import { StandardMaterial } from "@babylonjs/core/Materials/standardMaterial";
import { Color3 } from "@babylonjs/core/Maths/math.color";
import { Vector3 } from "@babylonjs/core/Maths/math.vector";
import { TransformNode } from "@babylonjs/core/Meshes/transformNode";
import type { Scene } from "@babylonjs/core/scene";

export type NpcRoutine = "commuter" | "vendor" | "patrol";

export class NpcActor {
  readonly root: TransformNode;
  reaction: "calm" | "alert" | "flee" = "calm";
  private waypointIndex = 0;
  private readonly speed: number;

  constructor(private readonly scene: Scene, private readonly waypoints: Vector3[], public readonly routine: NpcRoutine, index: number) {
    this.root = new TransformNode(`npc-${routine}-${index}`, scene);
    this.root.position.copyFrom(waypoints[0]);
    this.speed = routine === "vendor" ? 0.45 : routine === "patrol" ? 1.5 : 1.1;
    const bodyMat = new StandardMaterial(`npc-mat-${index}`, scene);
    bodyMat.diffuseColor = Color3.FromHexString(routine === "patrol" ? "#E9B949" : routine === "vendor" ? "#EA7B5A" : "#B4C7E7");
    const body = MeshBuilder.CreateCylinder(`npc-body-${index}`, { height: 1.1, diameter: 0.42, tessellation: 10 }, scene);
    body.parent = this.root;
    body.position.y = 0.58;
    body.material = bodyMat;
    const head = MeshBuilder.CreateSphere(`npc-head-${index}`, { diameter: 0.38, segments: 10 }, scene);
    head.parent = this.root;
    head.position.y = 1.35;
    head.material = bodyMat;
  }

  update(delta: number, playerPosition: Vector3, wanted: number): void {
    const target = this.waypoints[(this.waypointIndex + 1) % this.waypoints.length];
    const toTarget = target.subtract(this.root.position);
    const distance = toTarget.length();
    if (distance < 0.45) {
      this.waypointIndex = (this.waypointIndex + 1) % this.waypoints.length;
    } else {
      this.root.position.addInPlace(toTarget.normalize().scale(this.speed * delta));
      this.root.rotation.y = Math.atan2(toTarget.x, toTarget.z);
    }
    const playerDistance = Vector3.Distance(this.root.position, playerPosition);
    this.reaction = playerDistance < 3.4 && wanted > 0.35 ? (this.routine === "patrol" ? "alert" : "flee") : "calm";
  }

  dispose(): void { this.root.dispose(false, true); }
}
