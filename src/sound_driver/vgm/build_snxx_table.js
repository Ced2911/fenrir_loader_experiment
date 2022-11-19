const clk_35 = 3579545;

function sn_to_freq(r, clk_35) {
    return (clk_35 / (2 * r * 16)).toFixed(0)
}

function freq_to_oct_fns(freq) {
    let ref_fq = 440
    let delta
    delta = 1200 * Math.log2(freq / ref_fq)

    const oct = (delta / 1200)
    const fns = delta < 0 ? 1200 + (delta % 1200) : (delta % 1200)

    return { delta, oct: Math.floor(oct), fns: fns.toFixed(0) }
}

function build_table() {
    const freq = []
    for (let i = 0; i < 0x400; i++) {
        const f = sn_to_freq(i, clk_35);
        const d = freq_to_oct_fns(f)
        freq.push(
            {
                r: i,
                freq: f,
                ...d
            }
        )
    }

    return freq
}

console.log(JSON.stringify(build_table(), null, 2))

const fs = require('fs')
function str_c(v) {
    let { freq, oct, fns } = v
    if (!isFinite(oct) || isNaN(oct)) {
        oct = 8
    }
    if (!isFinite(fns) || isNaN(fns)) {
        fns = 0
    }
    return `/* freq: ${freq} */ ${oct}, ${fns}`
}
fs.writeFileSync("sn_map.h", build_table().map(k => str_c(k)).join(',\n'), () => { })