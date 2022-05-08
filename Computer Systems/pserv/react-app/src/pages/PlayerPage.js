import React, { useEffect, useState } from 'react';
import { useSelector, useDispatch } from 'react-redux';
import { Container } from 'reactstrap';
import RequireAuthentication from '../containers/RequireAuthentication';
import ReactPlayer from 'react-player/lazy';
import { isLoaded } from '../util/loadingObject'
import { list } from '../actions/video.js';
import { Col, Row, Label, Input, Button, ButtonToolbar, Form, FormGroup, Spinner } from 'reactstrap';
import { Dropdown, DropdownToggle, DropdownMenu, DropdownItem } from 'reactstrap';

const PlayerPage = () => {
  const [dropdownOpen, setDropdownOpen] = useState(false);
  const toggle = () => setDropdownOpen(prevState => !prevState);

  const dispatch = useDispatch();

  let videos = useSelector((state) => state.video);
  let [state, setState] = useState({
    url: ""
  });
  const handleChange = (event) => {
    setState({
      ...state,
      [event.target.name]: event.target.value
    })
  }

  useEffect(() => {
    dispatch(list());
  }, []);

  const [ playUrl, setPlayUrl ] = useState("");
  const handleSubmit = (event) => {
    event.preventDefault();
    setPlayUrl(state.url);
  }

  return (
    <Container>
      <Row>
        <h1>Welcome to the Player page</h1>
      </Row>
      <Row>
        <Form onSubmit={handleSubmit}>
          <Container>
            <Row>
              <Col>
                <FormGroup>
                  <Label for="url">Enter URL</Label>
                  <Input
                    type="text"
                    name="url"
                    value={state.url}
                    onChange={handleChange}
                  />
                </FormGroup>
                <ButtonToolbar>
                  <Button type="submit" bsstyle="success" className="mr-2">
                    Submit
                  </Button>
                </ButtonToolbar>
              </Col>
              <Col>
                or select one from the list of
                {!isLoaded(videos) ? (
                  <Spinner size="lg" color="primary" />
                ) : (
                  <Dropdown isOpen={dropdownOpen} toggle={toggle}>
                    <DropdownToggle caret>Available MP4s</DropdownToggle>
                    <DropdownMenu>
                      {videos.list.map((v) => (
                        <DropdownItem
                          key={v.name}
                          onClick={() => setPlayUrl(v.name)}
                        >
                          {v.name}
                        </DropdownItem>
                      ))}
                    </DropdownMenu>
                  </Dropdown>
                )}
              </Col>
            </Row>
          </Container>
        </Form>
      </Row>

      <Row className="mt-3">
        If your implementation of range byte requests works, you should be able
        to stream MP4 from your server.
      </Row>
      <Row className="mt-3">
        <ReactPlayer url={playUrl} controls={true} />
      </Row>
    </Container>
  );
};

export default RequireAuthentication(PlayerPage);
