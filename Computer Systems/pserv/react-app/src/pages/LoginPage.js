import React from 'react';
import { useDispatch, useSelector } from 'react-redux';

import { Card, CardHeader, CardBody, Container, Row, Col } from 'reactstrap';
import { login } from '../actions/auth.js';
import { isLoading, isLoaded } from '../util/loadingObject'
import LoginForm from '../components/forms/LoginForm';
import { Redirect } from 'react-router-dom';

const LoginPage = (props) => {
  const dispatch = useDispatch();
  function doLogin({username, password}) {
    dispatch(login(username, password));
  }

  const user = useSelector(state => state.auth);
  const isAuthenticated = isLoaded(user);
  const { from } = props.location.state || { from: { pathname: "/" } };
  if (isAuthenticated) {
    return (<Redirect to={from} />);
  }

  return (
    <Container>
      <Row className="pb-5 pt-5">
        <Col xsoffset={0} xs={10} smoffset={4} sm={4}>
          <Card>
            <CardHeader><h3>Please log in</h3></CardHeader>
            <CardBody>
              <LoginForm
                loading={isLoading(user)}
                autherror={user.error}
                onSubmit={v => doLogin(v)} />
            </CardBody>
          </Card>
        </Col>
      </Row>
    </Container>);
}

export default LoginPage;