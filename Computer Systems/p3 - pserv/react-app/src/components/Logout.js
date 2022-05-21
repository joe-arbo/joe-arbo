import React, { useEffect } from 'react';
import { Redirect } from 'react-router-dom';
import store from '../store';

const Logout = () => {
    // normally, we would inform the server just in case.
    document.cookie = "auth_token=";
    useEffect(() => {
        store.dispatch({ type: "LOGOUT" });
    }, []);
    return (<Redirect to="/" />);
};

export default Logout;
