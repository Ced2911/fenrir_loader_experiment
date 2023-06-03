<template>
  <div class="upload-area">
    <div class="file has-name is-fullwidth">
      <label class="file-label">
        <input class="file-input" type="file" @change="onDrop" />
        <span class="file-cta">
          <span class="file-icon">
            <font-awesome-icon icon="fa-solid fa-upload" />
          </span>
          <span class="file-label">Upload background</span>
        </span>
        <span class="file-name"> {{ filename }} </span>
      </label>
    </div>

    <slot></slot>
  </div>
</template>
<script lang="ts">
export default {
  emits: ['files-dropped'],
  methods: {
    onDrop(e: any) {
      this.filename = e.target.files[0].name
      this.$emit('files-dropped', e.target.files[0])
    }
  },
  data() {
    return { filename: '' }
  }
}
</script>
<style scoped></style>
