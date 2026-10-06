import { VertexData } from "@babylonjs/core/Meshes/mesh.vertexData";
import { Mesh } from "@babylonjs/core/Meshes/mesh";
import { PBRMaterial } from "@babylonjs/core/Materials/PBR/pbrMaterial";
import { Color3 } from "@babylonjs/core/Maths/math.color";
import { Scene } from "@babylonjs/core/scene";
import { WorldCountry } from "./worldRegistry";
import { CountryFeatureLayer } from "./CountryFeatureLayer";
import { pbrMaterial } from "../rendering/PbrMaterials";

type TerrainPackage = {
  schemaVersion: number;
  countryId: string;
  mapKey: string;
  biome: string;
  grid: {
    columns: number;
    rows: number;
    cellSizeM: number;
    originXM: number;
    originZM: number;
    heightsM: number[];
  };
  surface: { friction: number; waterLevelM: number; roadClearanceM: number };
  features: Parameters<CountryFeatureLayer["load"]>[0];
};

const TERRAIN_ROOT = `${import.meta.env.BASE_URL}world/terrain/`;

function terrainColor(biome: string): string {
  if (biome.includes("polar")) return "#9db9bd";
  if (biome.includes("rainforest")) return "#315e4a";
  if (biome.includes("volcanic")) return "#604e52";
  if (biome.includes("coastal")) return "#3f716f";
  if (biome.includes("highland")) return "#61715b";
  if (biome.includes("canyon")) return "#846d56";
  return "#4e765d";
}

export class CountryTerrain {
  private mesh: Mesh | null = null;
  private readonly material: PBRMaterial;
  private readonly featureLayer: CountryFeatureLayer;

  constructor(private readonly scene: Scene) {
    this.material = pbrMaterial(scene, "country-terrain-material", "#4e765d", { roughness: 0.88, metallic: 0.02, environmentIntensity: 0.85 });
    this.featureLayer = new CountryFeatureLayer(scene);
  }

  async load(country: WorldCountry): Promise<void> {
    if (country.countryId === "country-001") {
      this.mesh?.dispose(false, true);
      this.mesh = null;
      this.featureLayer.dispose();
      this.scene.getMeshByName("district-ground")?.setEnabled(true);
      return;
    }
    const response = await fetch(`${TERRAIN_ROOT}${country.name.toLowerCase()}.json`);
    if (!response.ok) throw new Error(`Terrain package unavailable for ${country.name}`);
    const terrain = (await response.json()) as TerrainPackage;
    if (terrain.countryId !== country.countryId) throw new Error(`Terrain identity mismatch for ${country.name}`);
    const grid = terrain.grid;
    if (grid.columns < 2 || grid.rows < 2 || grid.heightsM.length !== grid.columns * grid.rows) {
      throw new Error(`Invalid heightfield for ${country.name}`);
    }

    const positions: number[] = [];
    const indices: number[] = [];
    for (let z = 0; z < grid.rows; z += 1) {
      for (let x = 0; x < grid.columns; x += 1) {
        const index = z * grid.columns + x;
        positions.push(grid.originXM + x * grid.cellSizeM, grid.heightsM[index], grid.originZM + z * grid.cellSizeM);
      }
    }
    for (let z = 0; z < grid.rows - 1; z += 1) {
      for (let x = 0; x < grid.columns - 1; x += 1) {
        const topLeft = z * grid.columns + x;
        const topRight = topLeft + 1;
        const bottomLeft = topLeft + grid.columns;
        const bottomRight = bottomLeft + 1;
        indices.push(topLeft, bottomLeft, topRight, topRight, bottomLeft, bottomRight);
      }
    }
    const vertexData = new VertexData();
    vertexData.positions = positions;
    vertexData.indices = indices;
    const normals: number[] = [];
    VertexData.ComputeNormals(positions, indices, normals);
    vertexData.normals = normals;

    const next = new Mesh(`terrain-${country.countryId}`, this.scene);
    vertexData.applyToMesh(next, true);
    this.material.albedoColor = Color3.FromHexString(terrainColor(terrain.biome));
    next.material = this.material;
    next.receiveShadows = true;
    next.metadata = { countryId: country.countryId, mapKey: country.mapKey, biome: terrain.biome, friction: terrain.surface.friction };
    this.mesh?.dispose(false, true);
    this.mesh = next;
    this.featureLayer.load(terrain.features);
    this.scene.getMeshByName("district-ground")?.setEnabled(false);
  }

  dispose(): void {
    this.mesh?.dispose(false, true);
    this.mesh = null;
    this.featureLayer.dispose();
    this.material.dispose();
  }
}
