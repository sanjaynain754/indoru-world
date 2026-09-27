export type Action = "forward" | "back" | "left" | "right" | "brake" | "save" | "load" | "reset" | "worldMap";

type DrivingAction = Extract<Action, "forward" | "back" | "left" | "right" | "brake">;

const bindings: Record<string, Action> = {
  KeyW: "forward",
  ArrowUp: "forward",
  KeyS: "back",
  ArrowDown: "back",
  KeyA: "left",
  ArrowLeft: "left",
  KeyD: "right",
  ArrowRight: "right",
  Space: "brake",
};

const drivingActions = new Set<Action>(["forward", "back", "left", "right", "brake"]);

export class InputManager {
  private readonly held = new Set<Action>();
  private readonly pressed = new Set<Action>();
  private readonly onKeyDown = (event: KeyboardEvent) => {
    const action = bindings[event.code];
    if (action) {
      event.preventDefault();
      if (!this.held.has(action)) this.pressed.add(action);
      this.held.add(action);
      return;
    }
    if (event.code === "KeyE") this.pressed.add("save");
    if (event.code === "KeyL") this.pressed.add("load");
    if (event.code === "KeyR") this.pressed.add("reset");
    if (event.code === "KeyM") this.pressed.add("worldMap");
  };
  private readonly onKeyUp = (event: KeyboardEvent) => {
    const action = bindings[event.code];
    if (action) this.held.delete(action);
  };
  private readonly onBlur = () => this.clearHeld();

  constructor(private readonly target: Window = window) {
    target.addEventListener("keydown", this.onKeyDown, { passive: false });
    target.addEventListener("keyup", this.onKeyUp);
    target.addEventListener("blur", this.onBlur);
  }

  isHeld(action: Action): boolean {
    return this.held.has(action);
  }

  setHeld(action: DrivingAction, active: boolean): void {
    if (active) this.held.add(action);
    else this.held.delete(action);
  }

  consume(action: Action): boolean {
    const wasPressed = this.pressed.has(action);
    this.pressed.delete(action);
    return wasPressed;
  }

  clearPressed(): void {
    this.pressed.clear();
  }

  private clearHeld(): void {
    this.held.clear();
  }

  dispose(): void {
    this.target.removeEventListener("keydown", this.onKeyDown);
    this.target.removeEventListener("keyup", this.onKeyUp);
    this.target.removeEventListener("blur", this.onBlur);
    this.clearHeld();
    this.clearPressed();
  }
}
