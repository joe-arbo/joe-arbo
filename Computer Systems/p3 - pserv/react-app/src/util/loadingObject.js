// Courtesy: Scott Pruett

// manage the loading of objects from an external source
// specifically, an object can be in state 'loading', followed
// by either 'ok' or 'error'
 
export function loadingObject(object = {}) {
  return { ...object,
    loadingStatus: 'loading',
    error: undefined,
  };
}
export function loadedObject(object) {
  return { ...object,
    loadingStatus: 'ok',
    error: undefined,
  };
}
export function errorObject(object) {
  return { ...object,
    loadingStatus: 'error',
  };
}

export function isLoading(object) {
  return object.loadingStatus === 'loading';
}

export function isError(object) {
  return object.loadingStatus === 'error';
}

export function isLoaded(object) {
  return object.loadingStatus === 'ok';
}
