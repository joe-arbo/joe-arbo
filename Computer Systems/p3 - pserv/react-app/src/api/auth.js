import { APIResource, buildURL } from './resource.js';

export function login({username, password}) {
  return new APIResource(buildURL('/login')).post(
    { username, password }
  );
}

export function checklogin() {
  return new APIResource(buildURL('/login')).get();
}

export function logout() {
  return new APIResource(buildURL('/logout')).post();
}

