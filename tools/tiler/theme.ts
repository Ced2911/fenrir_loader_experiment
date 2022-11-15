enum DataType {
    u8,
    u16,
    u32,
    f16,
    s32,
    ptr,
    u16_x2,
    u16_x4,
    u16_x16,
}

const types: Record<string, Record<string, DataType>> = {}
const jsonables: Record<string, Record<string, any>> = {}

function ctype(type: DataType) {
    return (target: any, key: string) => {
        types[target.constructor.name] = types[target.constructor.name] || {}
        types[target.constructor.name][key] = type;

        jsonables[target.constructor.name] = jsonables[target.constructor.name] || {}
        jsonables[target.constructor.name][key] = null
    };
}

function jsonable(type: any) {
    return (target: any, key: string) => {
        jsonables[target.constructor.name] = jsonables[target.constructor.name] || {}
        jsonables[target.constructor.name][key] = type;
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
                this[k] = new (jsonables[ctr_name][k]);
                if (json[k]) {
                    this[k].parse(json[k])
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
        //console.log(types[this.constructor.name])

        //console.log(Object.keys(this))

        const childs = Object.keys(this)

        // const data_key = types[this.constructor.name] || {}
        for (const k of childs) {

            if (types[this.constructor.name] && types[this.constructor.name][k]) {

                const data_type = types[this.constructor.name][k]

                switch (data_type) {
                    case DataType.u8:
                        writer.writeU8(this[k])
                        break;
                    case DataType.u16:
                        writer.writeU16(this[k])
                        break;
                    case DataType.f16:
                        writer.writeU16(this[k])
                        break;
                    case DataType.u32:
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
                }
            } else {
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
    char_width: number = 0;

    @ctype(DataType.s32)
    char_height: number = 0

    @ctype(DataType.ptr)
    data: number[] = Array()

    @ctype(DataType.ptr)
    char_spacing: number[] = Array()
}

class UiConfigBackground extends CSerializable {
    @ctype(DataType.u8)
    screen: ThemeScr = ThemeScr.NBG0

    @ctype(DataType.u32)
    pattern_offset: number = 0;

    @ctype(DataType.u16)
    velocity_x: number = 0;

    @ctype(DataType.u16)
    velocity_y: number = 0;
}

class UiPositionBar extends CSerializable {
    @ctype(DataType.u16)
    w: number = 0;

    @ctype(DataType.u16)
    h: number = 0;

    @ctype(DataType.u16)
    color: number = 0;

    @ctype(DataType.u16_x4)
    gouraud: number[] = Array(4);

    @ctype(DataType.u16)
    enabled: boolean = false
}

class UiItemColor extends CSerializable {
    @ctype(DataType.u16_x2)
    colors: number[] = Array(2);

    @ctype(DataType.u16_x2)
    gouraud: number[] = Array(2);
}

class UiConfigBrowser extends CSerializable {
    @ctype(DataType.u16)
    x: number = 0;

    @ctype(DataType.u16)
    y: number = 0;

    @ctype(DataType.u16)
    w: number = 0;

    @ctype(DataType.u16)
    h: number = 0;

    @ctype(DataType.u16)
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
    x: number = 0;

    @ctype(DataType.u16)
    y: number = 0;

    @ctype(DataType.u16)
    w: number = 0;

    @ctype(DataType.u16)
    h: number = 0;

    @ctype(DataType.u16_x16)
    noise_palettes: number[] = Array(16);

    @ctype(DataType.u8)
    enabled: boolean = false
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

export class UiConfig extends CSerializable {
    @jsonable(UiConfigScreens)
    screens: UiConfigScreens

    @jsonable(UiConfigFont)
    main_font: UiConfigFont
}

