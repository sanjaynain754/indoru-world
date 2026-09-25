import { MeshBuilder } from "@babylonjs/core/Meshes/meshBuilder";
import { StandardMaterial } from "@babylonjs/core/Materials/standardMaterial";
import { Color3 } from "@babylonjs/core/Maths/math.color";
import { Vector3 } from "@babylonjs/core/Maths/math.vector";
import { TransformNode } from "@babylonjs/core/Meshes/transformNode";
import type { Scene } from "@babylonjs/core/scene";

export interface TrafficRoute { start: Vector3; end: Vector3; speed: number; color: string; label: string; }

export class TrafficCar {
  readonly root: TransformNode;
  private distance = 0;
  private readonly routeLength: number;
  readonly label: string;

  constructor(private readonly scene: Scene, private readonly route: TrafficRoute, offset: number) {
    this.root = new TransformNode(`traffic-${route.label}-${offset}`, scene);
    this.routeLength = Vector3.Distance(route.start, route.end);
    this.distance = offset % this.routeLength;
    this.label = route.label;
    const material = new StandardMaterial(`traffic-mat-${route.label}-${offset}`, scene);
    material.diffuseColor = Color3.FromHexString(route.color);
    const body = MeshBuilder.CreateBox(`traffic-body-${offset}`, { width: 1.7, height: 0.55, depth: 3.2 }, scene);
    body.parent = this.root;
    body.position.y = 0.55;
    body.material = material;
    const roof = MeshBuilder.CreateBox(`traffic-roof-${offset}`, { width: 1.35, height: 0.42, depth: 1.35 }, scene);
    roof.parent = this.root;
    roof.position.set(0, 0.98, -0.15);
    roof.material = material;
    this.syncPosition();
  }

  update(delta: number): void {
    this.distance = (this.distance + this.route.speed * delta) % this.routeLength;
    this.syncPosition();
  }

  private syncPosition(): void {
    const t = this.distance / this.routeLength;
    const position = Vector3.Lerp(this.route.start, this.route.end, t);
    this.root.position.copyFrom(position);
    this.root.rotation.y = Math.atan2(this.route.end.x - this.route.start.x, this.route.end.z - this.route.start.z);
  }

  distanceTo(point: Vector3): number { return Vector3.Distance(this.root.position, point); }
  dispose(): void { this.root.dispose(false, true); }
}
