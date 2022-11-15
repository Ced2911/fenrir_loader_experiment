
import { readFileSync, writeFileSync } from 'fs';
import { parse } from 'jsonc-parser'
import { UiConfig, BinaryWriter } from './theme';
import { RessourceImporter} from './ressources'

const themejson = parse(readFileSync('theme.jsonc').toString())

function bufferWriterBE(buffer: Buffer): BinaryWriter {
    let offset = 0;
    return {
        writeU8(v) {
            console.log(this)
            buffer.writeUInt8(v, offset)
            offset++
        },
        writeU16(v) {
            buffer.writeUInt16BE(v, offset)
            offset += 2
        }, writeU32(v) {
            buffer.writeUInt32BE(v, offset)
            offset += 4
        }, writeS8(v) {
            buffer.writeUInt8(v, offset)
            offset++
        }, writeS16(v) {
            buffer.writeInt16BE(v, offset)
            offset += 2
        }, writeS32(v) {
            buffer.writeInt32BE(v, offset)
            offset += 4
        },
    }
}

function bufferWriterLE(buffer: Buffer): BinaryWriter {
    let offset = 0;
    return {
        writeU8(v) {
            console.log(this)
            buffer.writeUInt8(v, offset)
            offset++
        },
        writeU16(v) {
            buffer.writeUInt16LE(v, offset)
            offset += 2
        }, writeU32(v) {
            buffer.writeUInt32LE(v, offset)
            offset += 4
        }, writeS8(v) {
            buffer.writeUInt8(v, offset)
            offset++
        }, writeS16(v) {
            buffer.writeInt16LE(v, offset)
            offset += 2
        }, writeS32(v) {
            buffer.writeInt32LE(v, offset)
            offset += 4
        },
    }
}


function main() {
    const buffer = Buffer.alloc(264) // sizeof(ui_config_t)
    const bwr = bufferWriterLE(buffer)
    let o = new UiConfig();
    o.parse(themejson)

    console.log(o)
    o.write(bwr)

    //console.log(jsonables)

    writeFileSync("theme.bin", buffer)

    // vdp2
    const resourceImporter = new RessourceImporter
    resourceImporter.importRessources(themejson.ressources)

    console.log(JSON.stringify(o, null, 2))
    console.log(buffer)

    console.log(resourceImporter.vdp2mem)
    console.log(resourceImporter.vdp2config)

}


main()