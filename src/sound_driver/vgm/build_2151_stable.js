function freq_table() {

}

function t(h) {
    const kf = 3584 + 64 * 12 * Math.log2(h / 440.0)
    const o = kf / 64 / 12
    const k = (kf / 64) % 12

    const notes_str = [
        'C',
        'C#',
        'D',
        'D#',
        'E',
        'F',
        'F#',
        'G',
        'G#',
        'A',
        'A#',
        'B',
    ];
    console.log(o, notes_str[Math.floor(k) + 1] + Math.floor(o), k)
}

function r(oct, kf) {
    const roct = oct * 64 * 12
    const rkf = kf * 64
    const hz = Math.pow(2, (((roct + rkf) - 3584) / (64 * 12))) * 440;

    return hz
}

// output all frequency 
function test_frequence() {
    const ar = []

    for (let oct = 0; oct < 9; oct++) {
        for (let kf = 0; kf < 12; kf++) {
            ar.push(r(oct, kf))
        }
    }

    console.log(JSON.stringify(ar.map(Math.floor), null, 2))
}



// https://github.com/commanderx16/x16-docs/blob/master/X16%20Reference%20-%2009%20-%20Sound%20Programming.md
function note_table() {
    // C4
    const scsp_note = 0;
    const scsp_oct = 4;



    // 
    const notes_str = {
        'C': 0,
        'C#': 1,
        'D': 2,
        'D#': 3,
        'E': 4,
        'F': 5,
        'F#': 6,
        'G': 7,
        'G#': 8,
        'A': 9,
        'A#': 10,
        'B': 11,
    };
    /*
        0	1	2	4	5	6	8	9	A	C	D	E
        c#	d	d#	e	f	f#	g	g#	a	a#	b	c
    */
    const note_map = [
        /*  0 */ notes_str['C#'], notes_str.D, notes_str['D#'], notes_str['D#'],
        /*  4 */ notes_str.E, notes_str.F, notes_str['F#'], notes_str['F#'],
        /*  8 */ notes_str.G, notes_str['G#'], notes_str.A, notes_str.A,
        /* 12 */ notes_str['A#'], notes_str.B, notes_str.C, notes_str.C
    ]
    const notes = Object.keys(notes_str)

    const cent_fcns = [...Array(12).keys()].map(v => {
        return Math.pow(2, 10) * (Math.pow(2, v * 100 / 1200) - 1)
    })

    // first note is c#4
    // remap array
    console.log(cent_fcns)
    const fns_c4 = cent_fcns.pop()
    cent_fcns.splice(0, 0, fns_c4)

    console.log(cent_fcns)


    const table = [...Array(16 * 8).keys()].map(v => {
        const note = note_map[v % 16];
        let oct = v >> 4
        if (note == notes_str.C) {
            oct++;
        }
        const fcn = note < 12 ? cent_fcns[note] : 0;
        return `/* ${notes[note]}${oct} - ${v.toString(16)}*/{${fcn.toFixed()}, 0x${((oct - scsp_oct) & 0xf).toString(16)}, "${notes[note]}${oct}"}`
    })

    return table.join(',\n')
}

const str = 'const scsp_kc_t scsp_kc_map[] = {' + note_table() + '};'


const fs = require('fs')
fs.writeFileSync("sound_table.h", str, () => { })