const tb = [
    
1712, 194,
1616, 205,
1525, 217,
1440, 230,
1357, 244,
1281, 259,
1209, 274,
1141, 291,
1077, 308,
1017, 326,
961, 345,
907, 366,
856, 388,
808, 411,
762, 436,
720, 461,
678, 490,
640, 519,
604, 550,
570, 583,
538, 617,
508, 654,
480, 692,
453, 733,
428, 776,
404, 822,
381, 872,
360, 923,
339, 980,
320, 1038,
302, 1100,
285, 1166,
269, 1235,
254, 1308,
240, 1385,
226, 1471,
214, 1553,
202, 1645,
190, 1749,
180, 1847,
170, 1955,
160, 2077,
151, 2201,
143, 2324,
135, 2462,
127, 2617,
120, 2770,
113, 2942,
107, 3107,
101, 3291,
95, 3499,
90, 3694,
85, 3911,
80, 4155,
76, 4374,
71, 4682,
67, 4962,
64, 5194,
60, 5541,
57, 5832,
]


function hz_to_pitch(hz) {
    let fns = 0;
    let oct = 0;
    let f = hz;
    let r = 0;

    if (hz < 44100) {
        // 8 / 16 / 32
        // oct
        while (f < 44100) {
            r = f
            f = f * 2
            oct--;
        }
        // fns
        f = hz * Math.pow(2, (-oct))
        console.log(f)
        fns = 1200
        m = f


        p = 1200
        FNS = Math.pow(2, 10) * (2 * Math.pow(p/1200)-1)

        console.log(FNS)
        while (m > 44100 & fns>0) {
            //m = f * 2 ^(100/1200)
            m = f * Math.pow(2, (fns/1200))
            fns--
            //console.log(fns, m)
            if (m<44100)
            break;
        }

    } else {
        //
    }

    return {fns, oct, f, r}
}
console.log(hz_to_pitch(9388))

if (0) {

const tbo = {}
for (i=0, j=0; i<tb.length; i+=2, j++) {
    hz = (tb[i+1] * 44100)>>12
    hz2 = 7159090 / (tb[i] * 2)
    note_frq = 7159090 / 64 / (tb[i]);
    tbo[tb[i]] = {psx:tb[i+1],hz:hz,note_frq:note_frq, hz2:hz2, sat:hz_to_pitch(hz)}

}

//

hz_to_pitch(44100/5)


to_c = []
for([k, v] of Object.entries(tbo)) {
    to_c.push(`/*${v.hz} hz / ${v.note_frq}*/   {.per=${k},.fns=${v.sat.fns},.oct=${v.sat.oct}}`)
}
console.log(to_c.join(",\n"))
}