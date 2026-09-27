import type { WorldSnapshot } from "../world/GameWorld";
import type { InputManager, Action } from "../input/InputManager";

export class HudController {
  private readonly root: HTMLDivElement;
  private readonly healthFill: HTMLDivElement;
  private readonly speed: HTMLDivElement;
  private readonly wanted: HTMLDivElement;
  private readonly mission: HTMLDivElement;
  private readonly missionFill: HTMLDivElement;
  private readonly status: HTMLDivElement;
  private readonly minimap: HTMLCanvasElement;
  private readonly mapContext: CanvasRenderingContext2D;
  private readonly worldMap: HTMLDivElement;
  private readonly waypoint: HTMLDivElement;
  private readonly waypointArrow: HTMLSpanElement;
  private readonly waypointDistance: HTMLSpanElement;
  private readonly touchControls: HTMLDivElement;
  private readonly touchListeners: Array<() => void> = [];

  constructor(input: InputManager) {
    this.root = document.createElement("div");
    this.root.className = "game-hud";
    this.root.innerHTML = `
      <div class="hud-topline"><div class="brand-lockup"><span class="brand-mark">I</span><div><b>INDORU</b><small>NAVAAR / RIVERFRONT DISTRICT</small></div></div><div class="save-help">WASD / ARROWS DRIVE <span>•</span> M WORLD MAP <span>•</span> E SAVE <span>•</span> L LOAD</div></div>
      <div class="hud-health panel"><div class="hud-label"><span>VITALS</span><strong data-health>100%</strong></div><div class="health-track"><div data-health-fill></div></div></div>
      <div class="hud-mission panel"><div class="mission-kicker">ACTIVE CONTRACT <span>01</span></div><div class="mission-title" data-mission>RIVERFRONT RUN</div><div class="mission-sub" data-mission-sub>Reach the beacon at Navaar Quay</div><div class="mission-track"><div data-mission-fill></div></div></div>
      <div class="hud-wanted panel"><div class="hud-label"><span>ATTENTION</span><strong data-wanted>QUIET</strong></div><div data-stars class="stars"></div></div>
      <div class="hud-speed"><strong data-speed>0</strong><span>KM/H</span></div>
      <div class="hud-waypoint"><span class="waypoint-arrow" data-waypoint-arrow>↑</span><div><b data-waypoint-distance>63 M</b><small>QUAY WAYPOINT</small></div></div>
      <div class="hud-bottom"><div class="city-chip"><span class="live-dot"></span><div><b>NAVAAR QUAY</b><small>AVENRA • INDORU WORLD</small></div></div><div class="agent-chip"><b data-agents>18 NPC / 15 TRAFFIC</b><small>SIMULATION ONLINE</small></div></div>
      <div class="hud-toast" data-status></div>
      <div class="touch-controls" aria-label="Touch driving controls">
        <div class="touch-steering"><button type="button" data-drive="left" aria-label="Steer left">◀</button><button type="button" data-drive="right" aria-label="Steer right">▶</button></div>
        <div class="touch-pedals"><button type="button" data-drive="brake" aria-label="Brake">BRAKE</button><button type="button" class="accelerate" data-drive="forward" aria-label="Accelerate">GO</button></div>
      </div>
      <canvas class="hud-minimap" width="168" height="168"></canvas>
      <div class="world-map-screen" data-world-map aria-hidden="true">
        <div class="world-map-card">
          <div class="world-map-header"><div><small>INDORU WORLD ATLAS</small><h2>THE SEVEN REACHES</h2></div><span>PRESS M TO CLOSE</span></div>
          <div class="island-map-graphic"><svg class="transport-network" viewBox="0 0 900 420" aria-hidden="true"><path class="bridge-route" d="M450 202 C335 175 230 140 120 94 M450 202 C570 164 700 120 818 92 M450 202 C350 270 260 340 180 364 M450 202 C560 275 675 337 790 363"/><path class="rail-route" d="M120 94 C270 35 620 35 818 92 M180 364 C400 408 640 400 790 363"/><path class="air-route" d="M120 94 Q450 20 818 92 M180 364 Q450 205 790 363 M120 94 Q155 210 180 364 M818 92 Q770 206 790 363"/></svg><span class="island-shape island-arvarra">AVARRA<br><small>STARTER</small></span><span class="island-shape island-khoruun">KHORUUN</span><span class="island-shape island-velmora">VELMORA</span><span class="island-shape island-orsik">ORSIK</span><span class="island-shape island-nembasa">NEMBASA</span><span class="island-shape island-dravik">DRAVIK</span><span class="island-shape island-erynd">ERYND</span><i class="map-pin pin-navaar"></i><b class="map-label label-navaar">NAVAAR<br><small>PLAYABLE NOW</small></b></div>
          <div class="world-map-footer"><div><b>AVENRA / INDORU</b><small>Starter country • 6 major cities • 201 settlements</small></div><div class="transport-facilities"><span class="facility-active">BRIDGE GRID</span><span>RAIL CORRIDORS</span><span>AIR ROUTES</span></div><div class="unlock-legend"><span class="legend-live"></span> ACTIVE <span class="legend-soon"></span> COMING SOON</div></div>
        </div>
      </div>`;
    document.body.appendChild(this.root);
    this.healthFill = this.root.querySelector("[data-health-fill]") as HTMLDivElement;
    this.speed = this.root.querySelector("[data-speed]") as HTMLDivElement;
    this.wanted = this.root.querySelector("[data-wanted]") as HTMLDivElement;
    this.mission = this.root.querySelector("[data-mission]") as HTMLDivElement;
    this.missionFill = this.root.querySelector("[data-mission-fill]") as HTMLDivElement;
    this.status = this.root.querySelector("[data-status]") as HTMLDivElement;
    this.minimap = this.root.querySelector(".hud-minimap") as HTMLCanvasElement;
    this.mapContext = this.minimap.getContext("2d") as CanvasRenderingContext2D;
    this.worldMap = this.root.querySelector("[data-world-map]") as HTMLDivElement;
    this.waypoint = this.root.querySelector(".hud-waypoint") as HTMLDivElement;
    this.waypointArrow = this.root.querySelector("[data-waypoint-arrow]") as HTMLSpanElement;
    this.waypointDistance = this.root.querySelector("[data-waypoint-distance]") as HTMLSpanElement;
    this.touchControls = this.root.querySelector(".touch-controls") as HTMLDivElement;
    this.touchControls.querySelectorAll<HTMLButtonElement>("[data-drive]").forEach((button) => {
      const action = button.dataset.drive as Action;
      const release = () => input.setHeld(action as "forward" | "back" | "left" | "right" | "brake", false);
      const press = (event: PointerEvent) => {
        event.preventDefault();
        button.setPointerCapture(event.pointerId);
        input.setHeld(action as "forward" | "back" | "left" | "right" | "brake", true);
      };
      button.addEventListener("pointerdown", press);
      button.addEventListener("pointerup", release);
      button.addEventListener("pointercancel", release);
      button.addEventListener("lostpointercapture", release);
      this.touchListeners.push(() => {
        button.removeEventListener("pointerdown", press);
        button.removeEventListener("pointerup", release);
        button.removeEventListener("pointercancel", release);
        button.removeEventListener("lostpointercapture", release);
        release();
      });
    });
  }

  update(snapshot: WorldSnapshot): void {
    this.healthFill.style.width = `${snapshot.health}%`;
    this.healthFill.dataset.danger = snapshot.health < 35 ? "true" : "false";
    this.speed.textContent = String(snapshot.speed).padStart(3, "0");
    this.mission.textContent = snapshot.missionStatus === "complete" ? "CONTRACT COMPLETE" : "RIVERFRONT RUN";
    (this.root.querySelector("[data-mission-sub]") as HTMLDivElement).textContent = snapshot.missionStatus === "complete" ? "Quay secured • Drive safe, Avenra" : "Reach the beacon at Navaar Quay";
    this.missionFill.style.width = `${Math.round(snapshot.missionProgress * 100)}%`;
    const stars = Math.ceil(snapshot.wanted);
    this.wanted.textContent = stars ? "PURSUIT ACTIVE" : "QUIET";
    this.wanted.dataset.hot = stars ? "true" : "false";
    const starsElement = this.root.querySelector("[data-stars]") as HTMLDivElement;
    starsElement.textContent = `${"★".repeat(stars)}${"☆".repeat(5 - stars)}`;
    starsElement.dataset.hot = stars ? "true" : "false";
    (this.root.querySelector("[data-agents]") as HTMLDivElement).textContent = `${snapshot.npcs} NPC / ${snapshot.traffic} TRAFFIC`;
    this.status.textContent = snapshot.savedFlash > 0 ? "LOCAL SAVE SYNCED" : "";
    this.status.classList.toggle("visible", snapshot.savedFlash > 0);
    const dx = -snapshot.player.x;
    const dz = -43 - snapshot.player.z;
    this.waypointDistance.textContent = snapshot.missionStatus === "complete" ? "ROUTE CLEAR" : `${Math.round(Math.hypot(dx, dz))} M`;
    const bearing = Math.atan2(dx, dz) * (180 / Math.PI);
    this.waypointArrow.style.transform = `rotate(${bearing - snapshot.heading}deg)`;
    this.waypoint.classList.toggle("complete", snapshot.missionStatus === "complete");
    this.worldMap.classList.toggle("open", snapshot.worldMapOpen);
    this.worldMap.setAttribute("aria-hidden", snapshot.worldMapOpen ? "false" : "true");
    this.drawMinimap(snapshot);
  }

  private drawMinimap(snapshot: WorldSnapshot): void {
    const ctx = this.mapContext;
    const width = this.minimap.width;
    ctx.clearRect(0, 0, width, width);
    ctx.fillStyle = "#091622";
    ctx.fillRect(0, 0, width, width);
    ctx.strokeStyle = "#213c4e";
    ctx.lineWidth = 5;
    for (const offset of [28, 72, 116, 150]) { ctx.beginPath(); ctx.moveTo(offset, 0); ctx.lineTo(offset, width); ctx.stroke(); ctx.beginPath(); ctx.moveTo(0, offset); ctx.lineTo(width, offset); ctx.stroke(); }
    ctx.strokeStyle = "#1e6671";
    ctx.lineWidth = 2;
    ctx.beginPath(); ctx.moveTo(18, 150); ctx.lineTo(150, 18); ctx.stroke();
    const px = 84 + snapshot.player.x * 1.3;
    const pz = 84 + snapshot.player.z * 1.3;
    ctx.fillStyle = "#F5B94D";
    ctx.beginPath(); ctx.arc(Math.max(7, Math.min(161, px)), Math.max(7, Math.min(161, pz)), 4, 0, Math.PI * 2); ctx.fill();
    ctx.strokeStyle = "#F5B94D";
    ctx.beginPath(); ctx.arc(84, 84 - 43 * 1.3, 4, 0, Math.PI * 2); ctx.stroke();
  }

  dispose(): void {
    this.touchListeners.forEach((cleanup) => cleanup());
    this.root.remove();
  }
}
