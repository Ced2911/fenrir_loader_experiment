// schema used to build ui.config

enum DataType {
    u8 = 1,
    u16,
    u32,
    f16,
    s32,
    u16_x2,
    u16_x4,
    u16_x16,
    ptr,
}

// order is important for export

const types: Record<string, Record<string, DataType>> = {}
const jsonables: Record<string, Record<string, any>> = {}
const jsonables_info: Record<string, Record<string, any>> = {}
const c_struct_types: Record<string, Record<string, string>> = {}
const c_struct_size: Record<string, Record<string, number>> = {}

const props_order: Record<string, Array<string>> = {}

function structPropsType(type: string, size?: number) {
    return (target: any, key: string) => {
        c_struct_types[target.constructor.name] = c_struct_types[target.constructor.name] || {}
        c_struct_types[target.constructor.name][key] = type;


        if (size) {
            c_struct_size[target.constructor.name] = c_struct_size[target.constructor.name] || {}
            c_struct_size[target.constructor.name][key] = size;
        }
    }
}

function ctype(type: DataType) {
    return (target: any, key: string) => {
        types[target.constructor.name] = types[target.constructor.name] || {}
        types[target.constructor.name][key] = type;

        jsonables[target.constructor.name] = jsonables[target.constructor.name] || {}
        jsonables[target.constructor.name][key] = null

        props_order[target.constructor.name] = props_order[target.constructor.name] || []
        props_order[target.constructor.name].push(key);
    };
}

function jsonable(type: any, info?: any) {
    return (target: any, key: string) => {
        jsonables[target.constructor.name] = jsonables[target.constructor.name] || {}
        jsonables[target.constructor.name][key] = type;


        jsonables_info[target.constructor.name] = jsonables_info[target.constructor.name] || {}
        jsonables_info[target.constructor.name][key] = info;

        props_order[target.constructor.name] = props_order[target.constructor.name] || []
        props_order[target.constructor.name].push(key);
    };
}

export interface BinaryWriter {
    writeU8(v: number): void,
    writeU16(v: number): void,
    writeU32(v: number): void,
    writeS8(v: number): void,
    writeS16(v: number): void,
    writeS32(v: number): void,
}

class CSerializable {
    constructor() {

    }

    parse(json: any) {
        //   console.log('json', json)
        const ctr_name = this.constructor.name
        const keys = Object.keys(jsonables[ctr_name])
        for (const k of keys) {


            //    console.log('keys', keys)
            //console.log(k)
            // is it an object ?
            if (jsonables[ctr_name][k] !== null) {
                //   console.log(k, ' is type ', jsonables[ctr_name][k])
                //console.log('create obj', k, ctr_name)
                this[k] = new (jsonables[ctr_name][k]);
                if (json[k]) {
                    this[k].parse(json[k])
                } else {
                    this[k].parse({})
                }

                //console.log(jsonables[ctr_name][k])
            } else if (jsonables[ctr_name][k] === null) {
                //    console.log('set', k, 'to', json[k], json)
                this[k] = json[k]
            }
            //  if (json[keys])
        }
    }

    write(writer: BinaryWriter) {
        const childs = (props_order[this.constructor.name])
        for (const k of childs) {
            if (types[this.constructor.name] && types[this.constructor.name][k]) {

                const data_type = types[this.constructor.name][k]
                //console.log('write', this.constructor.name, this[k], k, data_type)

                switch (data_type) {
                    case DataType.u8:
                        writer.writeU8(this[k])
                        break;
                    case DataType.u16:
                        writer.writeU16(this[k])
                        break;
                    case DataType.u32:
                    case DataType.f16:
                        writer.writeU32(this[k])
                        break;
                    case DataType.s32:
                        writer.writeS32(this[k])
                        break;
                    case DataType.ptr:
                        writer.writeU32(this[k])
                        break;
                    case DataType.u16_x2:
                        writer.writeU16(this[k][0])
                        writer.writeU16(this[k][1])
                        break;
                    case DataType.u16_x4:
                        for (let i = 0; i < 4; i++)
                            writer.writeU16(this[k][i])
                        break;
                    case DataType.u16_x16:
                        for (let i = 0; i < 16; i++)
                            writer.writeU16(this[k][i])
                        break;
                    default:
                        console.error('type not handled')
                        break;
                }
            } else {
                if (Array.isArray(this[k])) {
                    this[k].forEach((elt) => {
                        if (elt.write)
                            elt.write(writer)
                    })
                }
                // console.log('write subj', k)
                if (this[k] && this[k].write) {
                    this[k].write(writer)
                }
            }
        }

    }
}



enum ThemeScr {
    NBG0 = 0,
    NBG2 = 2
}

class UiConfigFont extends CSerializable {
    @ctype(DataType.s32)
    @structPropsType('int')
    char_width: number = 0;

    @ctype(DataType.s32)
    @structPropsType('int')
    char_height: number = 0
}

class UiConfigBackground extends CSerializable {

    @ctype(DataType.u32)
    @structPropsType('uint32_t')
    pattern_offset: number = 0;

    @ctype(DataType.f16)
    @structPropsType('fix16_t')
    velocity_x: number = 0;

    @ctype(DataType.f16)
    @structPropsType('fix16_t')
    velocity_y: number = 0;

    @ctype(DataType.u8)
    @structPropsType('uint8_t')
    screen: ThemeScr = ThemeScr.NBG0
}

class UiPositionBar extends CSerializable {

    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    w: number = 0;

    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    h: number = 0;

    @ctype(DataType.u16_x2)
    @structPropsType('rgb1555_t', 2)
    color: number[] = Array(2);

    @ctype(DataType.u16_x4)
    @structPropsType('rgb1555_t', 4)
    gouraud: number[] = Array(4);

    @ctype(DataType.u8)
    @structPropsType('uint8_t')
    enabled: boolean = false


    @ctype(DataType.u8)
    @structPropsType('uint8_t')
    padding: number = 0

}

class UiItemColor extends CSerializable {
    @ctype(DataType.u16_x2)
    @structPropsType('rgb1555_t', 2)
    colors: number[] = Array(2);

    @ctype(DataType.u16_x4)
    @structPropsType('rgb1555_t', 4)
    gouraud: number[] = Array(4);
}

class UiConfigBrowser extends CSerializable {
    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    x: number = 0;

    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    y: number = 0;

    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    w: number = 0;

    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    h: number = 0;

    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    line_height: number = 0;

    @jsonable(UiPositionBar)
    position_bar: UiPositionBar

    @jsonable(UiItemColor)
    item_colors: UiItemColor

    @jsonable(UiItemColor)
    item_focused_colors: UiItemColor
}
class UiConfigScreenGameListCover extends CSerializable {
    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    x: number = 0;

    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    y: number = 0;

    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    w: number = 0;

    @ctype(DataType.u16)
    @structPropsType('uint16_t')
    h: number = 0;

    @ctype(DataType.u16_x16)
    @structPropsType('rgb1555_t', 16)
    noise_palettes: number[] = Array(16);

    @ctype(DataType.u8)
    @structPropsType('uint8_t')
    enabled: number = 0

    @ctype(DataType.u8)
    @structPropsType('uint8_t')
    padding: number = 0
}

class UiConfigScreenGameList extends CSerializable {
    @jsonable(UiConfigScreenGameListCover)
    cover: UiConfigScreenGameListCover

    @jsonable(UiConfigBrowser)
    browser: UiConfigBrowser

    @jsonable(UiConfigBackground)
    foreground: UiConfigBackground

    @jsonable(UiConfigBackground)
    background: UiConfigBackground
}
class UiConfigError extends CSerializable {
    @jsonable(UiConfigBackground)
    foreground: UiConfigBackground

    @jsonable(UiConfigBackground)
    background: UiConfigBackground
}
class UiConfigScreens extends CSerializable {
    @jsonable(UiConfigScreenGameList)
    gamelist: UiConfigScreenGameList

    @jsonable(UiConfigError)
    error_no_sd: UiConfigError

    @jsonable(UiConfigError)
    error_bad_filesystem: UiConfigError
}

class UiConfigVdp2Screen extends CSerializable {
    @ctype(DataType.u32)
    @structPropsType('uint32_t')
    cell_addr: number = 0

    @ctype(DataType.u32)
    @structPropsType('uint32_t')
    pal_addr: number = 0

    @ctype(DataType.u32)
    @structPropsType('uint32_t')
    pattern_addr: number = 0
}

class UiConfigVdp2 extends CSerializable {
    @jsonable(UiConfigVdp2Screen)
    nbg0: UiConfigVdp2Screen

    @jsonable(UiConfigVdp2Screen)
    nbg1: UiConfigVdp2Screen

    @jsonable(UiConfigVdp2Screen)
    nbg2: UiConfigVdp2Screen

    @jsonable(UiConfigVdp2Screen)
    nbg3: UiConfigVdp2Screen
}

export class UiConfigRessources extends CSerializable {
    @ctype(DataType.u32)
    @structPropsType('uint32_t')
    type: number = 0

    @ctype(DataType.u32)
    @structPropsType('uint32_t')
    offset: number = 0

    @ctype(DataType.u32)
    @structPropsType('uint32_t')
    lenght: number = 0
}

export class UiConfig extends CSerializable {
    @ctype(DataType.u32)
    @structPropsType('uint32_t')
    version: number = 0;

    @ctype(DataType.u32)
    @structPropsType('uint32_t')
    size: number = 0;

    @jsonable(UiConfigVdp2)
    vdp2: UiConfigVdp2

    @jsonable(UiConfigScreens)
    screens: UiConfigScreens

    @jsonable(UiConfigFont)
    main_font: UiConfigFont

    @ctype(DataType.u32)
    @structPropsType('uint32_t')
    ressource_count: number = 0

    //@structPropsType('uint8_t []')
    @jsonable(UiConfigRessources, { isArray: true })
    ressources: Array<UiConfigRessources> = []
}

function toCtype(k) {
    return k.replace(/(?:^|\.?)([A-Z])/g, function (x, y) { return "_" + y.toLowerCase() }).replace(/^_/, "") + '_t';

}

export function ToCStruct() {
    let c_str = `// autoconv\n`
    //console.log(jsonables)
    Object.entries(jsonables)
        .forEach(([k, v]) => {
            const c_type = toCtype(k)
            c_str += `typedef PACKED_STRUCT {\n`

            props_order[k].forEach(prop => {
                if (c_struct_types[k] && c_struct_types[k][prop]) {
                    const c_type = c_struct_types[k][prop]
                    let s = ''
                    if (c_struct_size[k] && c_struct_size[k][prop]) {
                        s = `[${c_struct_size[k][prop]}]`
                    }
                    c_str += `\t${c_type} ${prop}${s};\n`;
                } else {
                    // console.log(jsonables[k][prop])
                    const subtype = jsonables[k][prop].name
                    let m = ''
                    if (jsonables_info[k][prop] && jsonables_info[k][prop].isArray) {
                        m = '[]'
                    }
                    c_str += `\t${toCtype(subtype)} ${prop}${m};\n`;
                }
            })


            c_str += `} ${c_type};\n`;

        })

    return c_str;
}

function RGB888_RGB1555_INITIALIZER(a, r, g, b) { return RGB1555(a, r >> 3, g >> 3, b >> 3) }
function RGB1555(a, r, g, b) {
    return a << 15 | ((r & 0x1f) << 10) | ((g & 0x1f) << 5) | (b & 0x1f)
}
/*
console.log(RGB1555(1, 0x11, 0x11, 0x11))
console.log(RGB1555(1, 0x05, 0x05, 0x05))

console.log([RGB1555(1, 0x12, 0x12, 0x12),
RGB1555(1, 0x12, 0x12, 0x12),
RGB1555(1, 0x08, 0x08, 0x08),
RGB1555(1, 0x08, 0x08, 0x08),])

console.log([RGB1555(1, 0x11, 0x11, 0x11),
RGB1555(1, 0x0, 0x0, 0x0),])
console.log([RGB888_RGB1555_INITIALIZER(1, 0xdd, 0x33, 0x11),
    RGB888_RGB1555_INITIALIZER(1, 0xdd, 0x33, 0x11),
    RGB888_RGB1555_INITIALIZER(1, 0xff, 0xcc, 0x11),
    RGB888_RGB1555_INITIALIZER(1, 0xff, 0xcc, 0x11),
])

console.log([RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3),     
    RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), 
    RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), 
    RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), 
                                            
    RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), 
    RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), 
    RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), 
    RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), 
                                            
    RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), 
    RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), 
    RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), 
    RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), 
                                            
    RGB1555(1, 0x00, 0xC0 >> 3, 0x40 >> 3), 
    RGB1555(1, 0x00, 0xF0 >> 3, 0x00 >> 3), 
    RGB1555(1, 0x00, 0x80 >> 3, 0x60 >> 3), 
    RGB1555(1, 0x00, 0xF0 >> 3, 0xF0 >> 3), 
])
*/