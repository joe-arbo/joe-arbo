import React from 'react';
import { Container, Row, Col } from 'reactstrap';
import { Link } from 'react-router-dom';
import logo from '../images/logo.svg';
import cs3214 from '../images/cs3214.png';

let HomePage = (props) => (
  <Container>
    <Row>
      <h1>CS3214 Demo App</h1>
    </Row>
    <Row>
      <Col>
        <img alt="" src={logo} className="app-logo" />
      </Col>
      <Col>
        <img alt="" src={cs3214} className="cs3214-logo" />
      </Col>
    </Row>
    <Row>
      <Col>
        <p>
          This small <a href="https://reactjs.org/">React {React.version}</a>{" "}
          app shows how to use the JWT authentication facilities of your server
          in a progressive single-page web application.
        </p>
      </Col>
    </Row>
    <Row>
      <Col>
        Click <Link to={`/protected`}>here</Link> to navigate to a protected
        section of the app.
      </Col>
    </Row>
  </Container>
);

export default HomePage
