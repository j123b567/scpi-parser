---
layout: page
title: Basic usage
order: 10
---

<ul>
{% assign pages_list = site.basic | sort:"order" %}
{% for p in pages_list %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a>
{% endfor %}
</ul>
