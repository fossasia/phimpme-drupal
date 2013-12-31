README
===================

This is a brief introduction to how the rest server works. See the [services_oop][services_oop] module to find out more about how you easily can expose functionality in a resource-oriented way.

All this depends on the functionality added by the [oauth-rest branch of services][oauth-rest].

Controllers
-------------------

Tabulation of the controller mapping for the REST server. Requests gets mapped to different controllers based on the HTTP method used and the number of parts in the path.

Count refers to the number of path parts that comes after the path that identifies the resource type. The request for `/services/rest/node/123` would have the count 1, as `/services/rest/node` identifies the resource.

    X = CRUD
    A = Action
    T = Targeted action
    R = Relationship request

    COUNT |0|1|2|3|4|N|
    -------------------
    GET   |X|X|R|R|R|R|
    -------------------
    POST  |X|A|T|T|T|T|
    -------------------
    PUT   | |X| | | | |
    -------------------
    DELETE| |X| | | | |
    -------------------

CRUD
-------------------

The basis of the REST server.

    Create:   POST /services/rest/node + body data
    Retrieve: GET /services/rest/node/123
    Update:   PUT /services/rest/node/123 + body data
    Delete:   DELETE /services/rest/node/123

And last but least, the little bastard sibling to Retrieve that didn't get it's place in the acronym: 

    Index:    GET /services/rest/node

In the REST server the index often doubles as a search function. The comment resource allows queries like the following for checking for new comments on a node (where 123456 is the timestamp for the last check and 123600 is now):

    New comments: GET /services/comment?nid=123&timestamp=123456:
    Comments in the last hour: GET /services/comment?timestamp=120000:123600

Actions
-------------------

Actions are performed directly on the resource type, not a individual resource. The following example is hypothetical (but plausible). Say that you want to expose a API for the [apachesolr][apachesolr] module. One of the things that could be exposed is the functionality to reindex the whole site.

    Publish:  POST /services/rest/apachesolr/reindex

Targeted actions
-------------------

Targeted actions acts on a individual resource. A good, but again - hypothetical, example would be the publishing and unpublishing of nodes. 

    Publish:  POST /services/rest/node/123/publish

Relationships
-------------------

Relationship requests are convenience methods (sugar) to get something thats related to a individual resource. A real example would be the relationship that the [comment_resource][comment_resource] module adds to the node resource:

    Get comments: GET /services/rest/node/123/comments

This more or less duplicates the functionality of the comment index:

    Get comments: GET /services/rest/comments?nid=123

[apachesolr]: http://drupal.org/project/apachesolr "Apache Solr Search Integration"
[comment_resource]: http://github.com/hugowetterberg/comment_resource "Comment resource"
[services_oop]: http://github.com/hugowetterberg/services_oop "Services OOP"
[oauth-rest]: http://github.com/hugowetterberg/services/tree/oauth-rest "Services oAuth REST branch"