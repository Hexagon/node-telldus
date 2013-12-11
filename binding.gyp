{
  "targets": [
    {
    "target_name": "telldus",
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
        }],
        ['OS == "win"', {
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
