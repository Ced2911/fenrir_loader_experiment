<script setup lang="ts"></script>

<template>
  <main class="container">
    <section class="hero">
      <div class="hero-body">
        <p class="title">Fenrir Configuration</p>
        <p class="subtitle">blah blah
        </p>
      </div>
    </section>
<div class=" is-flex">
    <aside class="section">
      <p class="menu-label">Screens</p>
      <ul class="menu-list">
        <li><a>Gamelist</a></li>
        <li><a>/!\ Error</a></li>
      </ul>
      <p class="menu-label">Sounds</p>
      <ul class="menu-list">
        <li><a>/!\ Sounds effects</a></li>
        <li><a>/!\ Music</a></li>
      </ul>
      <p class="menu-label">Configuration</p>
      <ul class="menu-list">
        <li><a>/!\ Wi-Fi</a></li>
      </ul>
    </aside>

    <div class="section">
      <router-view></router-view>
    </div></div>
  </main>
</template>

<style scoped></style>
