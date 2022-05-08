import React from 'react';
import { useSelector } from 'react-redux';
import { Container, Row } from 'reactstrap';
import RequireAuthentication from '../containers/RequireAuthentication';

const PrivatePage = () => {
  let user = useSelector(state => state.auth);

  return (<Container>
    <h1>Welcome to a private page</h1>
    <Row className="mt-3">
      You have successfully authenticated as user&nbsp;<span>{user.sub}</span>.
    </Row>
    <Row className="mt-3">
      Your token was issued at {new Date(user.iat*1000).toString()}, 
      it expires {new Date(user.exp*1000).toString()}
    </Row>
    <Row className="mt-3">
      This page is "private" only inasmuch as the front-end does not
      display it to unauthenticated users.  In a fully-fledged app,
      this page would now perform API requests that require authentication.
    </Row>
  </Container>);
}

export default RequireAuthentication(PrivatePage);
