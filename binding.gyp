{
  "targets": [
    {
    "target_name": "telldus-core-js",
    "sources": [ "telldus.cc" ],
    "conditions": [
        ['OS=="mac"', {
            'include_dirs': [
            	'/Library/Frameworks/TelldusCore.framework/Headers'
            ],
            'libraries': [
            	'/Library/Frameworks/TelldusCore.framework'
            ]
        }],
        ['OS=="linux"', {
        	'link_settings': {
         		'libraries': [
         			'-ltelldus-core',
         		]
        	}
        }]
      ]
   }
  ]
}