<script lang="ts">
export default {
  components: {},
  setup() {
    // expose to template and other options API hooks
    return {}
  },

  props: {
    modelValue: {
      type: Object,
      required: true
    },
    label: String
  },

  emits: ['update:modelValue'],
  data() {
    return { ...this.modelValue }
  },
  methods: {
    updateBackgroundAnimation() {
      this.$emit('update:modelValue', { x_inc: this.x_inc, y_inc: this.y_inc })
    }
  },
  mounted() {}
}
</script>

<template>
  <div class="background-settings">
    <label class="label"> {{label}}</label>
    <div class="field is-horizontal">
      <div class="field-body">
        <div class="field has-addons">
          <div class="control">
            <span class="button is-static"> X </span>
          </div>
          <div class="control">
            <input
              @change="updateBackgroundAnimation"
              step="0.25"
              type="number"
              class="input"
              v-model.number="x_inc"
            />
          </div>
        </div>
        <div class="field has-addons">
          <div class="control">
            <span class="button is-static"> Y </span>
          </div>
          <div class="control">
            <input
              @change="updateBackgroundAnimation"
              step="0.25"
              type="number"
              class="input"
              v-model.number="y_inc"
            />
          </div>
        </div>
      </div>
    </div>
    <div></div>
  </div>
</template>
<style lang="scss" scoped></style>
