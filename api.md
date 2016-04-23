---
layout: page
title: API Documentation
order: 20
---

{% assign pages_list = site.api | sort:"order" %}

Parameters handling
-------

<ul>
{% for p in pages_list %}
  {% if p.category == "parameters" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a></li>
  {% endif %}
{% endfor %}
</ul>

Generating results
-------

<ul>
{% for p in pages_list %}
  {% if p.category == "results" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a></li>
  {% endif %}
{% endfor %}
</ul>

Generating results as array
-------

<ul>
{% for p in pages_list %}
  {% if p.category == "results_array" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a></li>
  {% endif %}
{% endfor %}
</ul>

Helper function for generating results
-------

<ul>
{% for p in pages_list %}
  {% if p.category == "results_helper" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a></li>
  {% endif %}
{% endfor %}
</ul>

Handling expressions
-------

<ul>
{% for p in pages_list %}
  {% if p.category == "expressions" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a></li>
  {% endif %}
{% endfor %}
</ul>

Extended parameters handling
-------

<ul>
{% for p in pages_list %}
  {% if p.category == "parameters_ex" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a></li>
  {% endif %}
{% endfor %}
</ul>

Command handling
-------

<ul>
{% for p in pages_list %}
  {% if p.category == "commands" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a></li>
  {% endif %}
{% endfor %}
</ul>

Parser functions
-------

<ul>
{% for p in pages_list %}
  {% if p.category == "parser" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a></li>
  {% endif %}
{% endfor %}
</ul>

Errors handling
-------

<ul>
{% for p in pages_list %}
  {% if p.category == "errors" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a></li>
  {% endif %}
{% endfor %}
</ul>

IEEE 488 registers handling
-------

<ul>
{% for p in pages_list %}
  {% if p.category == "regs" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a></li>
  {% endif %}
{% endfor %}
</ul>
