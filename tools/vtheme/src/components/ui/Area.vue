<script lang="ts">
import VueResizable from '@/components/Resizable.vue'

import { Area } from '@/models/screens'

export default {
  components: {
    VueResizable
  },
  setup() {
    // expose to template and other options API hooks
    return {}
  },

  props: {
    area: {
      type: Object,
      required: true
    },
    active: Boolean
  },

  emits: ['update'],
  data() {
    const area_rs = {
      width: this.area?.w,
      height: this.area?.h,
      left: this.area?.x,
      top: this.area?.y,
      maxWidth: 512,
      maxHeight: 512,
      minWidth: 10,
      minHeight: 10,
      fitParent: true
    }
    return {
      area_rs
    }
  },
  computed: {
    ractive() {
      return this.active ? ['r', 'rb', 'b', 'lb', 'l', 'lt', 't', 'rt'] : []
    }
  },
  methods: {
    eHandler(data: any) {
      const o = Object.assign({}, this.area)
      o.h = data.height
      o.w = data.width
      o.x = data.left
      o.y = data.top
      this.$emit('update', o)
    }
  },
  mounted() {}
}
</script>

<template>
  <VueResizable v-bind="area_rs" @resize:end="eHandler" @drag:end="eHandler" :active="ractive">
    <div class="resizable-content" :class="{ active: active }">
      <slot></slot>
    </div>
  </VueResizable>
</template>
<style lang="scss" scoped>
.fenrir-ui-browser {
  position: relative;
}
.resizable-content {
  height: 100%;
  width: 100%;
  position: relative;
  &.active {
    &::before {
      content: ' ';
      //background-color: aqua;
      //opacity: 0.2;
      border: 1px solid #555;
      width: 100%;
      height: 100%;
      display: block;
      position: absolute;
    }
  }
}
</style>
