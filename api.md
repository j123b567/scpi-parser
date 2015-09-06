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
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a>
  {% endif %}
{% endfor %}
</ul>

Generating results
-------
<ul>
{% for p in pages_list %}
  {% if p.category == "results" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a>
  {% endif %}
{% endfor %}
</ul>

Extended parameters handling
-------
<ul>
{% for p in pages_list %}
  {% if p.category == "parameters_ex" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a>
  {% endif %}
{% endfor %}
</ul>

Command handling
-------
<ul>
{% for p in pages_list %}
  {% if p.category == "commands" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a>
  {% endif %}
{% endfor %}
</ul>

Parser functions
-------
<ul>
{% for p in pages_list %}
  {% if p.category == "parser" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a>
  {% endif %}
{% endfor %}
</ul>

Errors handling
-------
<ul>
{% for p in pages_list %}
  {% if p.category == "errors" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a>
  {% endif %}
{% endfor %}
</ul>

IEEE 488 registers handling
-------
<ul>
{% for p in pages_list %}
  {% if p.category == "regs" %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a>
  {% endif %}
{% endfor %}
</ul>
