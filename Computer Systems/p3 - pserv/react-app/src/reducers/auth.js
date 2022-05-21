/* Reducer.
 *
 * Internal state is as follows:
 * {
 *      auth:   {
 *          sub:
 *          iat:  ...
 *          exp:   ...
 *          loadingStatus:   ok/loading/error
 *      }
 * }
 */
import asyncHandler from './asyncHandler';

// we could get the initial state also via an <script ....> in public/index.html
let initialState = { };

const loginHandler = asyncHandler('LOGIN', initialState);
const checkloginHandler = asyncHandler('CHECKLOGIN', initialState);

export default function(state = initialState, action) {
    let newState = state;
    if (action.type.startsWith('LOGIN:')) {
        newState = loginHandler(state, action);
    }
    if (action.type === 'CHECKLOGIN:OK') {
        // a bit hacky.
        newState = checkloginHandler(state, action);
        if (!('sub' in newState))
            delete newState.loadingStatus;
    }
    if (action.type === 'LOGOUT') {
        newState = { }
    }

    return { ...newState };
}
