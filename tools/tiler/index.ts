
import { readFileSync, writeFileSync } from 'fs';
import { parse } from 'jsonc-parser'
import { UiConfig, BinaryWriter, ToCStruct, UiConfigRessources } from './theme';
import { RessourceImporter } from './ressources'
import { off } from 'process';

const themejson = parse(readFileSync('theme.jsonc').toString())

function bufferWriterBE(buffer: Buffer): BinaryWriter {
    let offset = 0;
    return {
        writeU8(v) {
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


async function main() {
    const buffer = Buffer.alloc(512) // sizeof(ui_config_t)
    const bwr = bufferWriterBE(buffer)
    let uiConfig = new UiConfig();
    uiConfig.parse(themejson)


    // vdp2
    const resourceImporter = new RessourceImporter
    await resourceImporter.importRessources(themejson.ressources)

    // merge ressources in config
    Object.entries(resourceImporter.vdp2config).forEach(([r, offset]) => {
        const [k, type, vdpscreen, ...screens] = r.split('-')

        screens.forEach(scr => {
            console.log('offset', offset)
            uiConfig.screens[scr][type].pattern_offset = offset
        })

    })

    // fill vdp2 config
    console.log(resourceImporter.vdp2mem)
    console.log(resourceImporter.vdp2config)

    uiConfig.vdp2.nbg0.cell_addr = resourceImporter.vdp2mem.cells.nbg0.addr;
    uiConfig.vdp2.nbg1.cell_addr = resourceImporter.vdp2mem.cells.nbg1.addr;
    uiConfig.vdp2.nbg2.cell_addr = resourceImporter.vdp2mem.cells.nbg2.addr;
    uiConfig.vdp2.nbg3.cell_addr = resourceImporter.vdp2mem.cells.nbg3.addr;
    
    uiConfig.vdp2.nbg0.pattern_addr = resourceImporter.vdp2mem.patterns.nbg0.addr;
    uiConfig.vdp2.nbg1.pattern_addr = resourceImporter.vdp2mem.patterns.nbg1.addr;
    uiConfig.vdp2.nbg2.pattern_addr = resourceImporter.vdp2mem.patterns.nbg2.addr;
    uiConfig.vdp2.nbg3.pattern_addr = resourceImporter.vdp2mem.patterns.nbg3.addr;
    
    uiConfig.vdp2.nbg0.pal_addr = resourceImporter.vdp2mem.palettes?.nbg0.addr;
    uiConfig.vdp2.nbg1.pal_addr = resourceImporter.vdp2mem.palettes?.nbg1.addr;
    uiConfig.vdp2.nbg2.pal_addr = resourceImporter.vdp2mem.palettes?.nbg2.addr;
    uiConfig.vdp2.nbg3.pal_addr = resourceImporter.vdp2mem.palettes?.nbg3.addr;

    uiConfig.ressources = []

    const vdp2vmem = resourceImporter.vdp2mem.vmem.slice(0, resourceImporter.vdp2mem.size)
    const vdp2cmem = resourceImporter.vdp2mem.cmem

    let offset = 0;

    uiConfig.ressources.push(Object.assign(new UiConfigRessources, {
        offset: offset,
        lenght: vdp2vmem.length,
        type: 1
    }))

    offset += vdp2vmem.length;

    uiConfig.ressources.push(Object.assign(new UiConfigRessources, {
        offset: offset,
        lenght: vdp2cmem.length,
        type: 2
    }))

    uiConfig.ressource_count = uiConfig.ressources.length;

    uiConfig.write(bwr)

    const finalBufferSize = buffer.length + vdp2vmem.length + vdp2cmem.length
    const finalBuffer = Buffer.concat([buffer, vdp2vmem, vdp2cmem], finalBufferSize)
    
    writeFileSync("theme.bin", finalBuffer)
}

main()
writeFileSync('struct.h.tmp', ToCStruct())