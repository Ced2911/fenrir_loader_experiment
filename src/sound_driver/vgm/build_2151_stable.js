function note_table() {
    // C4
    const scsp_note = 0;
    const scsp_oct = 4;

    // 
    const notes = [
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
        'PADDING',        
        'PADDING',        
        'PADDING',
        'PADDING'
    ];

    const cent_fcns = [...Array(12).keys()].map(v=> {
        return Math.pow(2,10)*(Math.pow(2,v*100/1200)-1)
    })


    const table = [...Array(16*8).keys()].map(v=> {
        const note = v % 16;
        const oct = v >> 4
        const fcn = note < 12 ? cent_fcns[note]: 0;
        return `/* ${notes[note]}${oct} */{${fcn.toFixed()}, 0x${((oct-scsp_oct)&0xf).toString(16)}}`
    })

    return table.join(',\n')
}

console.log('const scsp_kc_t scsp_kc_map[] = {')
console.log(note_table())
console.log('};')