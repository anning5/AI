// namespace help us to group classes and avoid clashes
namespace octet {

	class character
	{
		vec3 *vertices;
		vec2 *uvs;
		vec3 pos;
		vec3 target_pos;
		vec3 map_start_pos;
		vec3 dir;
		a_star &as;
		a_star_app *app;
		float grid_width;
		float speed;
		unsigned int target_path_node;
		a_star::grid *current_grid;
		bool moving;
	public:

		character(a_star &as) :
			speed(.01f),
			target_path_node(0),
			as(as),
			vertices(NULL),
			uvs(NULL),
			app(NULL),
			current_grid(NULL),
			moving(false),
			pos(vec3(999, 999, 999))
		{
			vertices = new vec3[4];
			uvs = new vec2[4];
			uvs[0] = vec2(0, 0);
			uvs[1] = vec2(.5f, 0);
			uvs[2] = vec2(.5f, .5f);
			uvs[3] = vec2(0, .5f);
		}

		~character()
		{
			if(vertices != NULL)
			{
				delete vertices;
			}
			if(uvs != NULL)
			{
				delete uvs;
			}
		}

		void set_pos(int x, int y)
		{
			pos = map_start_pos + vec3(x * grid_width, y * grid_width, map_start_pos.z());
		}

		void update(float t);

		void reset();

		void set_start()
		{
			moving = false;
		}

		void set_map_info(const vec3 &map_start_pos, float grid_width)
		{
			this->grid_width = grid_width;
			this->map_start_pos = map_start_pos;
		}

		a_star::grid* get_next_grid()
		{
			return as.get_grid(as.get_grid_count() - 1 - target_path_node++);
		}

		void set_app(a_star_app *app)
		{
			this->app = app;
		}

		void render()
		{
			vertices[0] = pos;
			vertices[1] = vec3(pos.x() + grid_width, pos.y(), 0);
			vertices[2] = vec3(pos.x() + grid_width, pos.y() + grid_width, 0);
			vertices[3] = vec3(pos.x(), pos.y() + grid_width, 0);
			glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices);
			glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)uvs);
			glDrawArrays(GL_QUADS, 0, 4);
		}

		void set_speed(float v)
		{
			speed = v;
		}
	};

	// this is an app to draw a triangle, it takes some of its workings from the class "app"
	class a_star_app : public app {
		vec3 *vertices;
		vec2 *uvs;
		int grid_width;
		unsigned int key_cool_down;
		a_star as;
		character c;
		map_generator map_gen;
		a_star_shader shader_;
		// Matrix to transform points on our triangle to the world space
		// This allows us to move and rotate our triangle
		mat4t modelToWorld;

		// Matrix to transform points in our camera space to the world.
		// This lets us move our camera
		mat4t cameraToWorld;
		vec3 camera_pos;
		GLuint tex;



	public:

		void draw_world(int x, int y, int w, int h);

		// this is called when we construct the class
		a_star_app(int argc, char **argv) :
			app(argc, argv),
			grid_width(50),
			vertices(NULL),
			uvs(NULL),
			as(grid_width),
			c(as),
			map_gen(grid_width),
			key_cool_down(0),
			camera_pos(0, 0, 5)
		{
			c.set_app(this);
			vertices = new vec3[4 * grid_width * grid_width];
			uvs = new vec2[4 * grid_width * grid_width];
		}

		~a_star_app()
		{
			if(vertices != NULL)
			{
				delete vertices;
			}
			if(uvs != NULL)
			{
				delete uvs;
			}
		}

		void set_grid_uv(int x, int y, float u, float v, float w)
		{
			if(x > grid_width - 1 || y > grid_width - 1)
				return;
			int index = (y * grid_width + x) * 4;
			uvs[index].get()[0] = u;
			uvs[index++].get()[1] = v;
			uvs[index].get()[0] = u + w;
			uvs[index++].get()[1] = v;
			uvs[index].get()[0] = u + w;
			uvs[index++].get()[1] = v + w;
			uvs[index].get()[0] = u;
			uvs[index].get()[1] = v + w;
		}

		void clear_red_dot(int i, int j)
		{
			set_grid_uv(i, j, .5f, .5f, .5f);
		}

		void ray_casting(int &grid_x, int &grid_y)
		{
			int mouse_x, mouse_y, w, h;
			get_mouse_pos(mouse_x, mouse_y);
			get_viewport_size(w, h);
			float x = (float)mouse_x / w * 2 - 1;
			float y = (float)mouse_y / h * -2 + 1;
			float x1 = x, y1 = y, tx, ty;
			vec3 dir = normalize(vec3(x1, y1, 4) - camera_pos);
			tx = -camera_pos.z() / dir.z() * dir.x();
			ty = -camera_pos.z() / dir.z() * dir.y();
			grid_x = (int)((tx - (-camera_pos.z())) / (camera_pos.z() - (-camera_pos.z())) * grid_width);
			grid_y = (int)((ty - (-camera_pos.z())) / (camera_pos.z() - (-camera_pos.z())) * grid_width);
		}

		void clear_map()
		{
			for(int i = 0; i < grid_width; i++)
			{
				for(int j = 0; j < grid_width; j++)
				{
					if(map_gen.get_grid(i, j) == 1)
					{
						set_grid_uv(i, j, .5f, .5f, .5f);
					}
					else
					{
						as.set_block(i, j);
						set_grid_uv(i, j, 0, .5f, .5);
					}
				}
			}
		}

		void update_map()
		{
			map_gen.clear();
			map_gen.bsp(0, 0, grid_width, grid_width, 6);
			clear_map();
		}

		void find_path()
		{
			as.search();
			const dynarray<a_star::grid*> &path = as.get_path();
			for(unsigned int i = 0; i < path.size(); i++)
			{
				set_grid_uv(path[i]->get_x(), path[i]->get_y(), .5f, 0, .5f);
			}
		}

		// this is called once OpenGL is initialized
		void app_init() {
			c.set_map_info(vec3(-camera_pos.z(), -camera_pos.z(), 0), (camera_pos.z() - -camera_pos.z()) / grid_width);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, .5f);
			// initialize the shader
			tex = resources::get_texture_handle(GL_RGB, "texture.gif");

			shader_.init();

			// put the triangle at the center of the world
			modelToWorld.loadIdentity();

			// put the camera a short distance from the center, looking towards the triangle
			cameraToWorld.loadIdentity();
			cameraToWorld.translate(camera_pos.x(), camera_pos.y(), camera_pos.z());
			int index = 0;
			float z = camera_pos.z();
			float x = -z, y = x, step = 2 * z / grid_width;
			float color = 0;
			for(int i = 0; i < grid_width; i++)
			{
				for(int j = 0; j < grid_width; j++)
				{
					vertices[index].get()[0] = x;
					vertices[index].get()[1] = y;
					vertices[index++].get()[2] = 0;

					vertices[index].get()[0] = x + step;
					vertices[index].get()[1] = y;
					vertices[index++].get()[2] = 0;

					vertices[index].get()[0] = x + step;
					vertices[index].get()[1] = y + step;
					vertices[index++].get()[2] = 0;

					vertices[index].get()[0] = x;
					vertices[index].get()[1] = y + step;
					vertices[index++].get()[2] = 0;
					x += step;
				}
				y += step;
				x = -z;
			}
			update_map();
		}

	};

	// this is called to draw the world
	void a_star_app::draw_world(int x, int y, int w, int h) {
		static unsigned last_time = GetTickCount();
		unsigned int current_time = GetTickCount();
		if(current_time - key_cool_down > 200)
		{
			if(is_key_down('R'))
			{
				update_map();
				c.set_pos(999, 999);
				key_cool_down = current_time;
			}
			if(is_key_down(key_lmb))
			{
				int grid_x = 0, grid_y = 0;
				ray_casting(grid_x, grid_y);
				as.set_origin(grid_x, grid_y);
				c.set_pos(grid_x, grid_y);
				c.set_start();
				clear_map();
				key_cool_down = current_time;
			}
			else if(is_key_down(key_rmb))
			{
				int grid_x = 0, grid_y = 0;
				as.clear();
				clear_map();
				ray_casting(grid_x, grid_y);
				as.set_target(grid_x, grid_y);
				find_path();
				c.reset();
				key_cool_down = current_time;
			}
		}
		int time = current_time - last_time;
		while(time > 10)
		{
			c.update(10 / 1000.f);
			time -= 10;
		}
		c.update(time / 1000.f);
		last_time = current_time;
		// set a viewport - includes whole window area
		glViewport(x, y, w, h);

		// clear the background to black
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// build a projection matrix: model -> world -> camera -> projection
		// the projection space is the cube -1 <= x/w, y/w, z/w <= 1
		mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

		// spin the triangle by rotating about Z (the view direction)

		// set up opengl to draw flat shaded triangles of a fixed color
		shader_.render(modelToProjection);
		glEnableVertexAttribArray(attribute_pos);
		glEnableVertexAttribArray(attribute_color);
		glEnableVertexAttribArray(attribute_uv);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);

		// attribute_pos (=0) is position of each corner
		// each corner has 3 floats (x, y, z)
		// there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
		///*
		glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices);
		glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)uvs);
		glDrawArrays(GL_QUADS, 0, 4 * grid_width * grid_width);

		c.render();
		//*/
	}

	a_star::a_star(int size) :
		size(size),
		origin(NULL),
		target(NULL)
	{
		grids = new grid[size * size];
	}

	void a_star::set_block(int x, int y)
	{
		grids[y * size + x].set_v(999999);
	}

	void a_star::generate_path()
	{
		if(current == target)
		{
			grid *g = current;
			do
			{
				path.push_back(g);
				g = g->get_came_from();
			}while(g != NULL);
		}
	}

	void character::reset()
	{
		target_path_node = 0;
		moving = true;
		a_star::grid *g = get_next_grid();
		if(g == NULL)
		{
			pos = vec3(999, 999, 999);
			return;
		}
		pos = map_start_pos + vec3(g->get_x() * grid_width, g->get_y() * grid_width, map_start_pos.z());
		app->clear_red_dot(g->get_x(), g->get_y());
		current_grid = g = get_next_grid();
		if(g == NULL)
			return;
		target_pos = map_start_pos + vec3(g->get_x() * grid_width, g->get_y() * grid_width, map_start_pos.z());
		dir = (target_pos - pos).normalize();
	}

	void character::update(float t)
	{
		const float MIN_DISTANCE = .01f;
		if(!moving)
		{
			return;
		}
		if((target_pos - pos).squared() < MIN_DISTANCE * MIN_DISTANCE)
		{
			if(current_grid != NULL)
			{
				app->clear_red_dot(current_grid->get_x(), current_grid->get_y());
				as.set_origin(current_grid->get_x(), current_grid->get_y());
			}
			a_star::grid *g = get_next_grid();
			if(g == NULL)
			{
				moving = false;
				return;
			}
			current_grid = g;
			target_pos = map_start_pos + vec3(g->get_x() * grid_width, g->get_y() * grid_width, map_start_pos.z());
			dir = (target_pos - pos).normalize();
		}
		else
		{
			pos += dir * t;
		}
	}


}
