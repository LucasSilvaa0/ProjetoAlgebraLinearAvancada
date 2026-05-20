export interface MFloatInstance {
  add(other: MFloatInstance): MFloatInstance;
  sub(other: MFloatInstance): MFloatInstance;
  mul(other: MFloatInstance): MFloatInstance;
  div(other: MFloatInstance): MFloatInstance;
  toDouble(): number;
  toString(): string;
  exp: number;
  sign: number;
  delete(): void; // Required for memory management
}

export interface MFloatModule {
  MFloat: {
    new (): MFloatInstance;
    new (val: number): MFloatInstance;
    makeNan(): MFloatInstance;
    makeInf(sign: number): MFloatInstance;
    makeZero(): MFloatInstance;
  };
}

export default function createMFloatModule(): Promise<MFloatModule>;