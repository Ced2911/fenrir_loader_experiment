meta:
  id: fenrir-loader-theme
  file-extension: fltm
  endian: be
seq:
  - id: header
    contents: "FLTM"
  - id: versin
    type: u4
  - id: theme
    type: theme
  - id: vdp2config
    type: vdp2config
  - id: vdp2vram
    type: chunk
  - id: vdp2cram
    type: chunk
  - id: font
    type: chunk
types:
  theme:

  chunk:
    - id: len
      type: u4
    - id: data
      size: len