<script lang="ts">
import { RGBFunc } from '@/services/Utils'


export default {
  props: {
    label_color: String,
    colors: {
      type: Object,
      required: true
    }
  },
  emits: ['update:colors'],
  methods: {
    update() {
      const o = Object.assign({}, this.colors)

      o.main_colors.color = RGBFunc.fromHexString(this.itemColor.color)
      o.main_colors.shadow = RGBFunc.fromHexString(this.itemColor.shadow)
      o.gradient_colors.tl = RGBFunc.fromHexString(this.itemColor.tl)
      o.gradient_colors.tr = RGBFunc.fromHexString(this.itemColor.tr)
      o.gradient_colors.br = RGBFunc.fromHexString(this.itemColor.br)
      o.gradient_colors.bl = RGBFunc.fromHexString(this.itemColor.bl)
      
      console.log(o, this.colors)

      this.$emit('update:colors', o)
    }
  },
  data() {
    const itemColor = {
      color: RGBFunc.toHexString(this.colors.main_colors.color),
      shadow: RGBFunc.toHexString(this.colors.main_colors.shadow),
      tl: RGBFunc.toHexString(this.colors.gradient_colors.tl),
      tr: RGBFunc.toHexString(this.colors.gradient_colors.tr),
      br: RGBFunc.toHexString(this.colors.gradient_colors.br),
      bl: RGBFunc.toHexString(this.colors.gradient_colors.bl)
    }
    return { itemColor }
  }
}
</script>

<template>
  <div>
    <slot name="main-label"></slot>
    <div class="control">
      <input @change="update" v-model="itemColor.color" type="color" />
      <input @change="update" v-model="itemColor.shadow" type="color" />
    </div>
  </div>
  <div class="field">
    <slot name="gradient-label"></slot>
    <div class="control">
      <input @change="update" v-model="itemColor.tl" type="color" />
      <input @change="update" v-model="itemColor.tr" type="color" />
      <input @change="update" v-model="itemColor.br" type="color" />
      <input @change="update" v-model="itemColor.bl" type="color" />
    </div>
  </div>
</template>

<style scoped></style>
