import { MeshBuilder } from "@babylonjs/core/Meshes/meshBuilder";
import type { Material } from "@babylonjs/core/Materials/material";
import { Vector3 } from "@babylonjs/core/Maths/math.vector";
import { TransformNode } from "@babylonjs/core/Meshes/transformNode";
import type { Scene } from "@babylonjs/core/scene";
import { AdvancedWaterMaterial, registerAdvancedWaterShader } from "../rendering/AdvancedWaterMaterial";
import { pbrMaterial } from "../rendering/PbrMaterials";

type Point = { x: number; y: number; z: number };
type CountryFeatures = {
  rivers: Array<{ id: string; widthM: number; points: Point[] }>;
  lakes: Array<{ id: string; x: number; y: number; z: number; radiusM: number }>;
  mountains: Array<{ id: string; x: number; y: number; z: number; heightM: number; radiusM: number }>;
  roads: Array<{ id: string; class: string; points: Point[] }>;
  bridges: Array<{ id: string; x: number; y: number; z: number; lengthM: number }>;
  railways: Array<{ id: string; points: Point[] }>;
  settlements: Array<{ id: string; type: string; x: number; y: number; z: number; populationTier: string }>;
  airports: Array<{ id: string; x: number; y: number; z: number; runwayLengthM: number }>;
  ports: Array<{ id: string; x: number; y: number; z: number; capacity: string }>;
  militaryBases: Array<{ id: string; x: number; y: number; z: number }>;
};

const vec = (point: Point): Vector3 => new Vector3(point.x, point.y, point.z);

export class CountryFeatureLayer {
  private root: TransformNode | null = null;
  private readonly materials: Material[] = [];

  constructor(private readonly scene: Scene) {}

  private material(name: string, color: string, emissive = "#000000"): Material {
    const material = pbrMaterial(this.scene, name, color, { roughness: name.includes("road") ? 0.9 : 0.62, metallic: name.includes("bridge") ? 0.65 : 0.05, emissive: emissive !== "#000000" ? emissive : undefined });
    this.materials.push(material);
    return material;
  }

  load(features: CountryFeatures): void {
    this.dispose();
    this.root = new TransformNode("country-feature-layer", this.scene);
    const road = this.material("procedural-road", "#29343b");
    const rail = this.material("procedural-rail", "#697b80");
    const village = this.material("procedural-village", "#a67b54");
    const city = this.material("procedural-city", "#53828a", "#173d49");
    const bridge = this.material("procedural-bridge", "#c58e3d");
    const airport = this.material("procedural-airport", "#75848b");
    const military = this.material("procedural-military", "#59634e");
    const mountain = this.material("procedural-mountain", "#53655b");
    registerAdvancedWaterShader();
    const riverWater = new AdvancedWaterMaterial(this.scene, "procedural-river-water");
    const lakeWater = new AdvancedWaterMaterial(this.scene, "procedural-lake-water");
    this.materials.push(riverWater, lakeWater);

    features.mountains.forEach((item) => {
      const mesh = MeshBuilder.CreateCylinder(item.id, { diameterTop: 2, diameterBottom: item.radiusM * 2, height: item.heightM, tessellation: 8 }, this.scene);
      mesh.position.set(item.x, item.y + item.heightM / 2, item.z);
      mesh.material = mountain;
      mesh.parent = this.root;
    });
    features.rivers.forEach((item) => this.tube(`${item.id}-mesh`, item.points, Math.max(1.5, item.widthM / 2), riverWater));
    features.roads.forEach((item) => this.tube(`${item.id}-mesh`, item.points, item.class === "highway" ? 2.0 : 1.0, road));
    features.railways.forEach((item) => this.tube(`${item.id}-mesh`, item.points, 0.35, rail));
    features.lakes.forEach((item) => {
      const mesh = MeshBuilder.CreateCylinder(item.id, { diameter: item.radiusM * 2, height: 0.08, tessellation: 32 }, this.scene);
      mesh.position.set(item.x, item.y, item.z);
      mesh.material = lakeWater;
      mesh.parent = this.root;
    });
    features.bridges.forEach((item) => this.box(item.id, item.x, item.y, item.z, item.lengthM, 0.45, 5.5, bridge));
    features.settlements.forEach((item) => {
      const isCity = item.type === "capital" || item.populationTier === "large";
      const size = isCity ? 10 : item.populationTier === "medium" ? 5 : 3;
      const mesh = MeshBuilder.CreateBox(item.id, { width: size, height: size * 0.7, depth: size }, this.scene);
      mesh.position.set(item.x, item.y + size * 0.35, item.z);
      mesh.material = isCity ? city : village;
      mesh.parent = this.root;
    });
    features.airports.forEach((item) => {
      this.box(`${item.id}-runway`, item.x, item.y, item.z, 12, 0.12, item.runwayLengthM, airport);
      this.box(`${item.id}-terminal`, item.x, item.y + 1.5, item.z - item.runwayLengthM * 0.25, 12, 3, 10, airport);
    });
    features.ports.forEach((item) => {
      this.box(`${item.id}-dock`, item.x, item.y, item.z, item.capacity === "large" ? 28 : 14, 0.35, 5, bridge);
      const buoy = MeshBuilder.CreateCylinder(`${item.id}-buoy`, { diameter: 1.2, height: 2.8, tessellation: 10 }, this.scene);
      buoy.position.set(item.x + 10, item.y + 1.4, item.z);
      buoy.material = riverWater;
      buoy.parent = this.root;
    });
    features.militaryBases.forEach((item) => {
      this.box(`${item.id}-base`, item.x, item.y + 1.2, item.z, 18, 2.4, 14, military);
      this.box(`${item.id}-tower`, item.x, item.y + 7, item.z, 1.2, 12, 1.2, military);
    });
  }

  private tube(name: string, points: Point[], radius: number, material: Material): void {
    const mesh = MeshBuilder.CreateTube(name, { path: points.map(vec), radius, tessellation: 8, cap: 3 }, this.scene);
    mesh.material = material;
    mesh.parent = this.root;
  }

  private box(name: string, x: number, y: number, z: number, width: number, height: number, depth: number, material: Material): void {
    const mesh = MeshBuilder.CreateBox(name, { width, height, depth }, this.scene);
    mesh.position.set(x, y + height / 2, z);
    mesh.material = material;
    mesh.parent = this.root;
  }

  dispose(): void {
    this.root?.dispose(false, true);
    this.root = null;
    this.materials.splice(0).forEach((material) => material.dispose());
  }
}
