---
layout: page
title: Basic usage
---

<ul>
{% for p in site.basic %}
  <li><a href="{{ p.url }}">{{ p.title }}</a>
{% endfor %}
</ul>
