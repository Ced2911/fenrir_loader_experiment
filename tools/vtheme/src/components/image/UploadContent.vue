<template>
  <div>
    <input type="file" class="upload-area" @change="onDrop" />
    <slot></slot>
  </div>
</template>
<script lang="ts">
export default {
  emits: ['files-dropped'],
  methods: {
    onDrop(e:any) {
      const url = URL.createObjectURL(e.target.files[0]);
      this.$emit('files-dropped', url)
    }
  }
}
</script>
<style scoped>
.upload-area {
  background: #555;
  padding: 15px;
  width: 100%;
  color: white;
}
</style>
