import type { WorldSnapshot } from "../world/GameWorld";

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

  constructor() {
    this.root = document.createElement("div");
    this.root.className = "game-hud";
    this.root.innerHTML = `
      <div class="hud-topline"><div class="brand-lockup"><span class="brand-mark">I</span><div><b>INDORU</b><small>NAVAAR / RIVERFRONT DISTRICT</small></div></div><div class="save-help">WASD / ARROWS DRIVE <span>•</span> E SAVE <span>•</span> L LOAD</div></div>
      <div class="hud-health panel"><div class="hud-label"><span>VITALS</span><strong data-health>100%</strong></div><div class="health-track"><div data-health-fill></div></div></div>
      <div class="hud-mission panel"><div class="mission-kicker">ACTIVE CONTRACT <span>01</span></div><div class="mission-title" data-mission>RIVERFRONT RUN</div><div class="mission-sub">Reach the beacon at Navaar Quay</div><div class="mission-track"><div data-mission-fill></div></div></div>
      <div class="hud-wanted panel"><div class="hud-label"><span>ATTENTION</span><strong data-wanted>QUIET</strong></div><div data-stars class="stars"></div></div>
      <div class="hud-speed"><strong data-speed>0</strong><span>KM/H</span></div>
      <div class="hud-bottom"><div class="city-chip"><span class="live-dot"></span><div><b>NAVAAR QUAY</b><small>AVENRA • INDORU WORLD</small></div></div><div class="agent-chip"><b data-agents>18 NPC / 15 TRAFFIC</b><small>SIMULATION ONLINE</small></div></div>
      <div class="hud-toast" data-status></div>
      <canvas class="hud-minimap" width="168" height="168"></canvas>`;
    document.body.appendChild(this.root);
    this.healthFill = this.root.querySelector("[data-health-fill]") as HTMLDivElement;
    this.speed = this.root.querySelector("[data-speed]") as HTMLDivElement;
    this.wanted = this.root.querySelector("[data-wanted]") as HTMLDivElement;
    this.mission = this.root.querySelector("[data-mission]") as HTMLDivElement;
    this.missionFill = this.root.querySelector("[data-mission-fill]") as HTMLDivElement;
    this.status = this.root.querySelector("[data-status]") as HTMLDivElement;
    this.minimap = this.root.querySelector(".hud-minimap") as HTMLCanvasElement;
    this.mapContext = this.minimap.getContext("2d") as CanvasRenderingContext2D;
  }

  update(snapshot: WorldSnapshot): void {
    this.healthFill.style.width = `${snapshot.health}%`;
    this.healthFill.dataset.danger = snapshot.health < 35 ? "true" : "false";
    this.speed.textContent = String(snapshot.speed).padStart(3, "0");
    this.mission.textContent = snapshot.missionStatus === "complete" ? "CONTRACT COMPLETE" : "RIVERFRONT RUN";
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

  dispose(): void { this.root.remove(); }
}
