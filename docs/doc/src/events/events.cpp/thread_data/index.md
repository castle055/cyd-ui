---
layout: class
title: thread_data
owner: __MISSING__
brief: __MISSING__
tags:
  - class
defined_in_file: src/events/events.cpp
declaration: "\nstruct thread_data;"
ctor: unspecified
dtor: unspecified
fields:
  event_listeners:
    description: __MISSING__
    type: std::list<cydui::events::CEventListener *> *
  event_queue:
    description: __MISSING__
    type: std::deque<cydui::events::CEvent *> *
---
