// https://github.com/vampirefrog/fmtoy/blob/0de6703b3373eb5bf19fd3deaae889286f330c21/fmtoy_ym2203.c#L46
function freq_to_oct_fnum(hz) {
    const oct = (69 + 12 * Math.log2(hz / 440.0)) / 12 - 1
    const ofnum = (144 * hz * (1 << 19) / 4000000) / (1 << (oct - 1));
    const fnum = ((ofnum >> 8 & 0x07) << 8) | ofnum & 0xff;
    return { oct, fnum }
}

//oct:4 / fnum:1038 => a4
// https://i.imgur.com/hJZz7vk.png
function oct_fnum_to_hz(oct, fnum) {
    const roct = 1 + ((oct * 12) - 69) / 12
    const rfnum = fnum * (1 << (oct - 1))

    const hhh = rfnum / ((1 << 19) / 4000000) / 144

    //console.log(rfnum)
    console.log(hhh)

    const hz = Math.pow(2, roct) * 440
    return { hz }
}

/*
console.log(
    (144 * 440 * (1 << 19) / 4000000)
)
console.log(
    8304 /  ((1 << 19) / 4000000) / 144
)

console.log(freq_to_oct_fnum(440))
console.log(oct_fnum_to_hz(4, 1038))
*/

// 261 - 522 => oct 4
/*
console.log(freq_to_oct_fnum(392))
console.log(oct_fnum_to_hz(4, 924))
console.log(freq_to_oct_fnum(523))
console.log(oct_fnum_to_hz(4, 1233))
console.log(freq_to_oct_fnum(524))
console.log(oct_fnum_to_hz(5, 100))
*/

for (i = 20; i < 160; i += 1) {
    //console.log(freq_to_oct_fnum(i))
}

/*
function tt() {
    const freq = [261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88]
    const m = [0.03125, 0.0625, 0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 16.0, 32.0,]

}

*/
const mhzgen = (7.670453 * Math.pow(10, 6))
const mhz8 = (8 * Math.pow(10, 6))
const mhz4 = (4 * Math.pow(10, 6))

function fnumber(freq, cpu, oct) {
    const fn = (Math.pow(2, 20)) / cpu / Math.pow(2, oct - 1) * 144 * freq;
    return { fn, oct }
}

function tt(frequency, o, n) {
    const ratio = (Math.pow(2, 20)) / (4 * Math.pow(10, 6)) / Math.pow(2, o - 1) * 144
    const d = (144 * 440 * (Math.pow(2, 20)) / (4 * Math.pow(10, 6))) / Math.pow(2, 3)

    const s = 654 * (1 << 4)
    const f = 1038 * (1 << 4)

    return { s, f }
}
// (Math.pow(2, 20)) / (4 * Math.pow(10, 6)) * 144 => 1038*16/440
console.log(
    tt(272, 3, 2076)
)

function fbase(fnum, oct) {
    const p = fnum * (1 << oct - 1)
    return p
}
function fbaseToFrequence(fbase, cpuspd) {
    const r = Math.pow(2, 20) / cpuspd
    return (fbase / 144) / r
}

console.log(fnumber(220, mhz8, 3))
console.log(fnumber(392, mhz8, 4))
console.log(fnumber(440, mhz8, 4))
console.log(fnumber(440, mhz4, 4))

console.log(fnumber(261, mhzgen, 4))
console.log(fnumber(440, mhzgen, 4))

console.log(fbase(1081, 4))
console.log(fbase(1038, 4))
console.log(fbase(642, 4))

function fbase(fnum, oct) {
    const p = fnum * (1 << oct - 1)
    return p
}
function fbaseTo111111111Frequence(fbase, cpuspd) {
    const r = Math.pow(2, 20) / cpuspd
    const freq_ym2203_ratio = ((1 << 20) / (cpuspd)) * (1 << 16);
    //return (fbase / 144) / r
    const ratio_f = 1 / (144 * r);
    const ratio_b = (1 / (144 * ((1 << 20) / (cpuspd)))) * (1 << 16);
    console.log(fbase * ratio_f)
    return (fbase * ratio_b) >> 16;
} 2

console.log(
    fbaseToFrequence(fbase(1081, 4), mhzgen),
    fbaseToFrequence(fbase(1080, 4), mhzgen),
    fbaseToFrequence(fbase(1038, 4), mhz8),
    fbaseToFrequence(fbase(642, 4), mhzgen)
)

function xfo_table() {
    const tables = []
    for (let i = 0; i < 1200; i++) {
        tables.push(Math.pow(2, i / 1200))
    }

    //console.log(tables)
    return tables.map(t => t * (1 << 8)).map(Math.round)
}

const ooo=Array.from([...Array(10).keys()]).map(k=>261.63 * Math.pow(2, ((k-4)*12)/12)).map(t => t * (1 << 8)).map(Math.round)

console.log(ooo)

const t = `const uint32_t xfo[1200]= {${xfo_table().join(",")}};\n`+
`const uint32_t oct_f[]={${ooo}};\n
const uint32_t zizi_0[]={${xfo_table().map(k=>k*ooo[0]>>8)}};\n
`

const fs = require('fs')
fs.writeFileSync("xfo.h", t, () => { })