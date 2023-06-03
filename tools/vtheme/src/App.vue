<script setup lang="ts"></script>

<template>
  <main class="container">
    <section class="hero">
      <div class="hero-body">
        <p class="title">Fenrir Configuration</p>
        <p class="subtitle">blah blah</p>
      </div>
    </section>
    <div class="menu is-flex">
      <aside class="section">
        <p class="menu-label">Screens</p>
        <ul class="menu-list">
          <li><router-link active-class="is-active" to="/game-list">Gamelist</router-link></li>
          <li>
            <a class="has-text-danger-dark">
              <span class="icon"> <font-awesome-icon icon="fa-solid fa-exclamation" /></span>
              Error
            </a>
          </li>
        </ul>
        <p class="menu-label">Sounds</p>
        <ul class="menu-list">
          <li>
            <a class="has-text-danger-dark"
              ><span class="icon">
                <font-awesome-icon icon="fa-solid fa-hurricane" />
              </span>
              Sounds effects</a
            >
          </li>
          <li>
            <a class="has-text-danger-dark"
              ><span class="icon">
                <font-awesome-icon icon="fa-solid fa-music" />
              </span>
              Music</a
            >
          </li>
        </ul>
        <p class="menu-label">Configuration</p>
        <ul class="menu-list">
          <li>
            <a class="has-text-danger-dark"
              ><span class="icon">
                <font-awesome-icon icon="fa-solid fa-wifi" />
              </span>
              Wi-Fi</a
            >
          </li>
        </ul>
      </aside>

      <div class="section">
        <router-view></router-view>
      </div>
    </div>
  </main>
</template>

<style scoped></style>
