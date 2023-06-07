import './assets/main.scss'
import 'verte/dist/verte.css'

/* fontawesome */
import { library } from '@fortawesome/fontawesome-svg-core'
import { FontAwesomeIcon } from '@fortawesome/vue-fontawesome'
import { fas } from '@fortawesome/free-solid-svg-icons'
library.add(fas)

/* vue */
import { createRouter, createWebHistory, createWebHashHistory } from 'vue-router'
import { createPinia } from 'pinia'
import { createApp } from 'vue'
import App from './App.vue'


const router = createRouter({
  // 4. Provide the history implementation to use. We are using the hash history for simplicity here.
  history: createWebHashHistory(),
  routes: [
    {
      path: '/',
      component: () => import('@/pages/Home.vue'),
    },
    {
      path: '/game-list',
      component: () => import('@/pages/screens/GameList.vue'),
    },
  ],
})

const app = createApp(App)
app.use(router)
app.use(createPinia())
app.component('font-awesome-icon', FontAwesomeIcon)
app.mount('#app')
