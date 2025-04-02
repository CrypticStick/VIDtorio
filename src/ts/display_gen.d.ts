declare module "display_gen" {

    export interface MyModule extends EmscriptenModule {
        _Process_Image(data: number, width: number, height: number, spacing: number, res: number, fmt: number): number
        _Process_Init(data_len: number): number
        _Set_JS_Resize_Ptr(ptr: number): void
        addFunction(func: (...args: any[]) => any, signature?: string): number;
        UTF8ToString(ptr: number, maxBytesToRead?: number): string;
        canvas: HTMLCanvasElement
    }

    export default function Module(moduleOverrides?: Partial<MyModule>): Promise<MyModule>
}