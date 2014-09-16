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
          'defines': [
            '_WINDOWS=1',
          ],
          'link_settings': {
            'libraries': [
              '-lTelldusCore'
            ],
            'library_dirs': [
              'C:\\Program\\Telldus\\Development\\',
              'C:\\Program Files\\Telldus\\Development\\',
              'C:\\Program Files (x86)\\Telldus\\Development\\'
            ]
          },
          'include_dirs': [
            'C:\\Program\\Telldus\\Development\\',
            'C:\\Program Files\\Telldus\\Development\\',
            'C:\\Program Files (x86)\\Telldus\\Development\\'
          ]
        }]
      ]
    }
  ]
}
