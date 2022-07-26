/**
 * Generate table for math
 */
const fs = require('fs');

// sync with sat-math.h
const FIX32_INT_BITS = 22;
const FIX32_FRAC_BITS = (32 - FIX32_INT_BITS);
const SIN_TABLE_LEN = (1 << FIX32_FRAC_BITS); // number of element

function getFixedSin(v, d) {
    let s = Math.sin(v);
    return parseInt(Math.round(s * d));
}

let main = () => {
    console.log('SIN_TABLE_LEN', SIN_TABLE_LEN)

    let content = `#include <stdint.h>\n#include "sat-math.h"\nconst fix32 sintab32[${SIN_TABLE_LEN}] = {`;

    let inc = (2 * Math.PI) / SIN_TABLE_LEN;
    let v = 0;
    let d = 1 << FIX32_FRAC_BITS;
    let n = 512;

    console.log('div', d);
    console.log('inc', inc);
    console.log(Math.sin(inc) * d);

    let variances = 0;

    for (let i = 0; i < SIN_TABLE_LEN; i++ , v += inc) {
        let s = Math.sin(v);

        // make output more readable
        if ((i%8) == 0) {
            content += '\n\t';
        }
        content += `${Math.round(s * d)}, `;

        let fix32 = parseInt(Math.round(s * d)) / d;
        variances += Math.abs((fix32 * n)  - (s*n));
    }

    let t = Math.PI / 3;

    console.log(parseInt(getFixedSin(t, d)) / d);
    console.log(Math.sin(t).toFixed(6));


    console.log('avg variances', variances / SIN_TABLE_LEN);

    content += '\n};';
    fs.writeFile('../src/sat-math-table.c', content);
};


main();