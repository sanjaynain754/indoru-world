import { ShaderMaterial } from "@babylonjs/core/Materials/shaderMaterial";
import { Effect } from "@babylonjs/core/Materials/effect";
import { Color3 } from "@babylonjs/core/Maths/math.color";
import type { Scene } from "@babylonjs/core/scene";

const vertexShader = `
precision highp float;
attribute vec3 position;
attribute vec3 normal;
uniform mat4 world;
uniform mat4 worldViewProjection;
uniform float time;
varying vec3 vNormal;
varying vec3 vWorldPosition;
void main() {
  vec3 displaced = position;
  displaced.y += sin(position.x * 0.16 + time * 1.7) * 0.055;
  displaced.y += cos(position.z * 0.13 + time * 1.25) * 0.04;
  vec4 worldPosition = world * vec4(displaced, 1.0);
  vWorldPosition = worldPosition.xyz;
  vNormal = normalize(normal + vec3(0.0, 0.18 * cos(position.x * 0.16 + time * 1.7), 0.0));
  gl_Position = worldViewProjection * vec4(displaced, 1.0);
}
`;

const fragmentShader = `
precision highp float;
uniform vec3 cameraPosition;
uniform float time;
uniform vec3 deepColor;
uniform vec3 shallowColor;
varying vec3 vNormal;
varying vec3 vWorldPosition;
void main() {
  vec3 viewDirection = normalize(cameraPosition - vWorldPosition);
  float fresnel = pow(1.0 - max(dot(normalize(vNormal), viewDirection), 0.0), 3.0);
  float ripples = 0.5 + 0.5 * sin(vWorldPosition.x * 0.25 + vWorldPosition.z * 0.13 + time * 1.8);
  vec3 base = mix(deepColor, shallowColor, fresnel * 0.8 + ripples * 0.08);
  float highlight = pow(max(dot(reflect(-viewDirection, normalize(vNormal)), normalize(vec3(-0.4, 0.9, 0.25))), 0.0), 48.0);
  gl_FragColor = vec4(base + vec3(highlight * 0.55), 0.82);
}
`;

export class AdvancedWaterMaterial extends ShaderMaterial {
  private elapsed = 0;
  private readonly observer;

  constructor(scene: Scene, name: string) {
    super(name, scene, { vertex: "indoruWater", fragment: "indoruWater" }, {
      attributes: ["position", "normal"],
      uniforms: ["world", "worldViewProjection", "view", "cameraPosition", "time", "deepColor", "shallowColor"],
      needAlphaBlending: true,
    });
    this.backFaceCulling = false;
    this.alpha = 0.9;
    this.setColor3("deepColor", Color3.FromHexString("#063c58"));
    this.setColor3("shallowColor", Color3.FromHexString("#35bfd0"));
    this.setFloat("time", 0);
    this.observer = scene.onBeforeRenderObservable.add(() => {
      this.elapsed += scene.getEngine().getDeltaTime() / 1000;
      this.setFloat("time", this.elapsed);
    });
  }

  dispose(forceDisposeEffect?: boolean, forceDisposeTextures?: boolean): void {
    this.getScene().onBeforeRenderObservable.remove(this.observer);
    super.dispose(forceDisposeEffect, forceDisposeTextures);
  }
}

export function registerAdvancedWaterShader(): void {
  Effect.ShadersStore.indoruWaterVertexShader = vertexShader;
  Effect.ShadersStore.indoruWaterFragmentShader = fragmentShader;
}
