import type { Scene } from "@babylonjs/core/scene";
import { Vector3 } from "@babylonjs/core/Maths/math.vector";
import { InputManager } from "../input/InputManager";
import { NpcActor, type NpcRoutine } from "../actors/Npc";
import { PlayerVehicle } from "../actors/Vehicle";
import { TrafficCar, type TrafficRoute } from "../actors/Traffic";

export interface WorldSnapshot {
  speed: number;
  health: number;
  wanted: number;
  missionProgress: number;
  missionStatus: "active" | "complete";
  traffic: number;
  npcs: number;
  alertNpcs: number;
  savedFlash: number;
  worldMapOpen: boolean;
  player: { x: number; z: number };
}

interface SaveRecord {
  schemaVersion: 1;
  x: number;
  z: number;
  heading: number;
  health: number;
  wanted: number;
  missionProgress: number;
}

const SAVE_KEY = "indoru:navaar:save:v1";
const START = new Vector3(0, 0, 20);
const CHECKPOINT = new Vector3(0, 0, -43);

export class GameWorld {
  readonly player: PlayerVehicle;
  readonly checkpoint = CHECKPOINT.clone();
  private readonly traffic: TrafficCar[] = [];
  private readonly npcs: NpcActor[] = [];
  private missionProgress = 0;
  private missionStatus: "active" | "complete" = "active";
  private wanted = 0;
  private collisionCooldown = 0;
  private savedFlash = 0;
  private worldMapOpen = new URLSearchParams(window.location.search).has("map");
  private elapsed = 0;

  constructor(private readonly scene: Scene, private readonly input: InputManager) {
    this.player = new PlayerVehicle(scene, START.clone());
    this.spawnTraffic();
    this.spawnNpcs();
  }

  private spawnTraffic(): void {
    const routes: TrafficRoute[] = [
      { start: new Vector3(-43, 0, -21), end: new Vector3(43, 0, -21), speed: 10, color: "#E9B949", label: "taxi" },
      { start: new Vector3(43, 0, 0), end: new Vector3(-43, 0, 0), speed: 8, color: "#D6DDE8", label: "sedan" },
      { start: new Vector3(-43, 0, 21), end: new Vector3(43, 0, 21), speed: 7, color: "#E2735F", label: "van" },
      { start: new Vector3(-28, 0, 52), end: new Vector3(-28, 0, -52), speed: 6, color: "#8B9DF5", label: "bus" },
      { start: new Vector3(28, 0, -52), end: new Vector3(28, 0, 52), speed: 9, color: "#F2F4F8", label: "sedan" },
    ];
    routes.forEach((route, routeIndex) => {
      for (let i = 0; i < 3; i += 1) {
        this.traffic.push(new TrafficCar(this.scene, route, i * 17 + routeIndex * 6));
      }
    });
  }

  private spawnNpcs(): void {
    const loops = [
      new Vector3(-39, 0, -28), new Vector3(-22, 0, -28), new Vector3(-22, 0, -6), new Vector3(-39, 0, -6),
      new Vector3(21, 0, 8), new Vector3(39, 0, 8), new Vector3(39, 0, 30), new Vector3(21, 0, 30),
    ];
    const routines: NpcRoutine[] = ["commuter", "vendor", "patrol"];
    for (let i = 0; i < 18; i += 1) {
      const offset = (i * 2) % loops.length;
      this.npcs.push(new NpcActor(this.scene, loops.slice(offset).concat(loops.slice(0, offset)), routines[i % routines.length], i));
    }
  }

  update(delta: number, demo: boolean): void {
    this.elapsed += delta;
    this.collisionCooldown = Math.max(0, this.collisionCooldown - delta);
    this.savedFlash = Math.max(0, this.savedFlash - delta);
    if (this.input.consume("reset")) this.reset();
    if (this.input.consume("save")) this.save();
    if (this.input.consume("load")) this.load();
    if (this.input.consume("worldMap")) this.worldMapOpen = !this.worldMapOpen;
    this.player.update(delta, this.input, demo);
    this.traffic.forEach((car) => car.update(delta));
    this.npcs.forEach((npc) => npc.update(delta, this.player.root.position, this.wanted));
    this.resolveTrafficCollisions();
    this.updateMission();
    this.wanted = Math.max(0, this.wanted - delta * (this.missionStatus === "complete" ? 0.08 : 0.018));
  }

  private resolveTrafficCollisions(): void {
    if (this.collisionCooldown > 0 || Math.abs(this.player.speed) < 8) return;
    const hit = this.traffic.some((car) => car.distanceTo(this.player.root.position) < 2.6);
    if (!hit) return;
    this.collisionCooldown = 1.2;
    this.player.health = Math.max(0, this.player.health - 8);
    this.wanted = Math.min(5, this.wanted + 0.85);
    if (this.player.health <= 0) this.reset();
  }

  private updateMission(): void {
    const distance = Vector3.Distance(this.player.root.position, this.checkpoint);
    this.missionProgress = Math.max(0, Math.min(1, 1 - distance / 90));
    if (distance < 7) {
      this.missionStatus = "complete";
      this.missionProgress = 1;
      this.wanted = Math.max(0, this.wanted - 0.35);
    }
  }

  save(): void {
    const record: SaveRecord = {
      schemaVersion: 1,
      x: this.player.root.position.x,
      z: this.player.root.position.z,
      heading: this.player.heading,
      health: this.player.health,
      wanted: this.wanted,
      missionProgress: this.missionProgress,
    };
    localStorage.setItem(SAVE_KEY, JSON.stringify(record));
    this.savedFlash = 2;
  }

  load(): void {
    const raw = localStorage.getItem(SAVE_KEY);
    if (!raw) return;
    try {
      const record = JSON.parse(raw) as Partial<SaveRecord>;
      if (record.schemaVersion !== 1 || !Number.isFinite(record.x) || !Number.isFinite(record.z)) return;
      const x = record.x as number;
      const z = record.z as number;
      const health = typeof record.health === "number" && Number.isFinite(record.health) ? record.health : 100;
      const wanted = typeof record.wanted === "number" && Number.isFinite(record.wanted) ? record.wanted : 0;
      const missionProgress = typeof record.missionProgress === "number" && Number.isFinite(record.missionProgress) ? record.missionProgress : 0;
      const heading = typeof record.heading === "number" && Number.isFinite(record.heading) ? record.heading : 0;
      this.player.root.position.set(x, 0, z);
      this.player.heading = heading;
      this.player.health = Math.max(0, Math.min(100, health));
      this.wanted = Math.max(0, Math.min(5, wanted));
      this.missionProgress = Math.max(0, Math.min(1, missionProgress));
      this.savedFlash = 2;
    } catch {
      localStorage.removeItem(SAVE_KEY);
    }
  }

  reset(): void {
    this.player.reset(START.clone());
    this.wanted = 0;
    this.missionProgress = 0;
    this.missionStatus = "active";
  }

  snapshot(): WorldSnapshot {
    return {
      speed: Math.round(Math.abs(this.player.speed) * 3.6),
      health: Math.round(this.player.health),
      wanted: this.wanted,
      missionProgress: this.missionProgress,
      missionStatus: this.missionStatus,
      traffic: this.traffic.length,
      npcs: this.npcs.length,
      alertNpcs: this.npcs.filter((npc) => npc.reaction !== "calm").length,
      savedFlash: this.savedFlash,
      worldMapOpen: this.worldMapOpen,
      player: { x: this.player.root.position.x, z: this.player.root.position.z },
    };
  }

  dispose(): void {
    this.player.dispose();
    this.traffic.forEach((car) => car.dispose());
    this.npcs.forEach((npc) => npc.dispose());
  }
}
