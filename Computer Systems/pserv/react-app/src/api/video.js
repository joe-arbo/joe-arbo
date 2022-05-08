import { APIResource, buildURL } from './resource.js';

export function list() {
  return new APIResource(buildURL('/video')).get();
}