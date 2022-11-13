function freq_table() {

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