import { PBRMaterial } from "@babylonjs/core/Materials/PBR/pbrMaterial";
import { Color3 } from "@babylonjs/core/Maths/math.color";
import type { Scene } from "@babylonjs/core/scene";

export type PbrOptions = {
  metallic?: number;
  roughness?: number;
  environmentIntensity?: number;
  emissive?: string;
};

export function pbrMaterial(scene: Scene, name: string, color: string, options: PbrOptions = {}): PBRMaterial {
  const material = new PBRMaterial(name, scene);
  material.albedoColor = Color3.FromHexString(color);
  material.metallic = options.metallic ?? 0.05;
  material.roughness = options.roughness ?? 0.72;
  material.environmentIntensity = options.environmentIntensity ?? 0.7;
  material.useRadianceOverAlpha = true;
  material.useSpecularOverAlpha = true;
  if (options.emissive) {
    material.emissiveColor = Color3.FromHexString(options.emissive);
    material.emissiveIntensity = 0.65;
  }
  return material;
}
