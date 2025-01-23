# Survival Game Project (Early Development)

The **Survival Game Project**, developed using Engine version 5.5 is being developed with long-term plans for an immersive, dynamic survival experience. Since we are only in **pre-alpha development**, we are more focused on planning and prototyping systems than we are on making a fully playable product.

## Table of Contents

- [Project Goals](#project-goals)
- [Development Roadmap](#development-roadmap)
- [Prerequisites](#prerequisites)
- [Installation (Future)](#installation-future)
- [Contribution Guidelines](#contribution-guidelines)
- [License](#license)
- [Contact](#contact)

## Project Goals

1. **Robust Survival Mechanics**  
   - Introduce gathering, crafting, and resource management systems that require you to make strategic decisions.  
   - Add hunger, thirst, and fatigue features for realism.

2. **Dynamic World and Exploration**  
   - Create some kind of procedural generation framework for generating environments, weather patterns, and day/night cycles.  
   - Create distinct biomes, each with unique flora and fauna behaviors.
   - Create immersive game mechanics that will fit your role-play game preference.
   - Dynamic world interactions.

3. **Immersive AI and Encounters**  
   - Design AI-driven creatures and NPCs that interact with players and each other.
   - Introduce NPC Companion to help the players for gathering, building, crafting, etc.  
   - Introduce adaptive enemy behavior to keep gameplay challenging and engaging.

4. **Multiplayer and Cooperative Play**  
   - Plan for seamless local/online co-op modes.  
   - Establish a stable network architecture to support multiple concurrent players.

5. **Engine 5.5 Innovations**  
   - Leverage cutting-edge rendering features for visually stunning landscapes.  
   - Explore new physics and animation tools introduced in Engine 5.5.

## Development Roadmap

### Prototype Phase (Current)

1. **Resource Systems**: Experiment with core survival mechanics, such as resource collection, crafting, item management, and other essential systems.  
2. **Locomotion Basics**: Implement a basic locomotion system with thread-safe operations to ensure stable character movement.  
3. **Enemy AI Evaluation**: Evaluate AI frameworks and test basic enemy movement patterns.  
4. **Core Gameplay Loop**: Sketch out the primary game loop (spawn, gather, craft, survive).  
5. **Basic Environment Setup**: Draft simple environments to test collision, line of sight, and initial terrain generation.  
6. **Data Structures**: Plan and prototype data structures for inventory items, player stats, and in-game objects.  
7. **Minimal UI/UX**: Create placeholder interfaces for health, inventory, and resource displays.  
8. **Early Debug Tools**: Implement logging and debugging tools to quickly iterate on gameplay features.  
9. **Thread-Safety Checkpoints**: Regular code reviews focusing on concurrency safety for locomotion and other parallel systems.  
10. **First Pass Balancing**: Begin initial balancing of resource distribution and crafting costs.

### Pre-Alpha Phase

1. **Game Loop Refinement**: Flesh out basic gameplay loops, linking survival mechanics with inventory and crafting.  
2. **Advanced Inventory**: Focus on a more robust inventory system, including sorting, filtering, and stack management.  
3. **Expanded Locomotion States**: Experiment with new locomotion states (sprinting, climbing, swimming) and prepare for the next level of movement.  
4. **Basic Multiplayer**: Implement fundamental multiplayer functionalities and initial network testing for co-op.  
5. **Environment Prototypes**: Build out a variety of small-scale prototypes to test different biome concepts and lighting setups.  
6. **UI Enhancements**: Introduce basic menu systems, improved HUD elements, and user-friendly inventory interactions.  
7. **Audio Framework**: Add placeholder sounds for footsteps, crafting, and ambient background to enhance immersion.  
8. **Character Progression**: Begin designing experience or skill systems (e.g., leveling up crafting speed or stamina).  
9. **Collision & Physics Tweaks**: Fine-tune collision detection, ragdoll physics, and environmental physics interactions.  
10. **Initial Performance Profiling**: Conduct performance checks to identify bottlenecks in rendering and networking.

### Alpha Phase

1. **Content Expansion**: Introduce a broader range of items, enemies, and environmental assets to diversify gameplay.  
2. **Medium-Level Locomotion**: Enhance character movement with mid-tier features like dodging, vaulting, or stealth modes.  
3. **AI Optimization**: Improve AI creatures and NPC environmental behaviors, making them more reactive and efficient.  
4. **Utility AI Planning**: Prepare foundational plans for a Utility AI system to govern NPC Companion decision-making.  
5. **Balancing & Performance**: Refine AI systems, resource distribution, and overall performance optimization.  
6. **Dynamic Weather Systems**: Implement variable weather patterns that affect visibility, movement, and resource availability.  
7. **Enhanced Crafting Mechanics**: Add crafting tiers, recipe progression, and specialized workbenches for deeper gameplay.  
8. **World Events**: Experiment with timed or random events (e.g., meteor showers, plague outbreaks) to keep the experience dynamic.  
9. **Environmental Hazards**: Introduce traps, toxic areas, or extreme weather conditions that require unique survival strategies.  
10. **NPC Interactions**: Expand dialogue and trading systems for neutral or friendly NPCs, paving the way for more complex social mechanics.

### Beta Phase

1. **UI/UX Polish**: Refine menus, inventory screens, and HUD elements for a more seamless player experience.  
2. **Network Finalization**: Solidify all networking features, including matchmaking, hosting, and reconnection logic.  
3. **Graphical Enhancements**: Improve lighting, shadows, and post-processing effects to elevate visual fidelity.  
4. **Comprehensive Testing**: Conduct extensive internal and external playtests to gather feedback and identify bugs.  
5. **Bug Tracking & Fixes**: Establish a streamlined bug reporting system and prioritize fixes that impact core gameplay.  
6. **Sound Design Improvements**: Replace placeholder audio with higher-quality effects and custom music.  
7. **AI Polishing**: Tweak enemy and NPC AI behavior for difficulty balancing and more lifelike responses.  
8. **Localization**: Prepare the framework for multi-language support (UI text, subtitles, audio if feasible).  
9. **Controller Support**: Ensure compatibility with various controllers and input devices for broader accessibility.  

## Prerequisites

- **Engine version 5.5** (or above) for development.  
- **C++ 17** (or higher) recommended for any native code.  
- **Git** for version control and project collaboration.

## Installation (Future)

> **Note**: Detailed installation and build instructions will be provided once we have a functional prototype or a pre-alpha release available.


# Contribution Guidelines

## Create a New Branch

Use a clear and descriptive name for your branch (e.g., `feature/inventory-system` or `bugfix/fix-locomotion-thread-safety`):

```bash
git checkout -b feature/my-new-feature
```

## Make Your Changes

- Implement your feature or bug fix in the appropriate directories (e.g., `Source/`, `Content/`, `Config/`).
- Adhere to the project’s coding style and best practices.
- Include comments where necessary for clarity.

## Commit and Push

Write clear commit messages that describe your changes:

```bash
git commit -m "Add advanced inventory system with stack management"
```

Push to your fork:

```bash
git push origin feature/my-new-feature
```

## Open a Pull Request

1. Go to your fork on GitHub and click “Compare & pull request.”
2. Provide a descriptive title and explanation of your changes, referencing any related issues or pull requests.
3. If possible, attach screenshots or GIFs demonstrating your additions or fixes.

## Code Review and Iterations

- Project maintainers will review your Pull Request, suggesting changes or asking for further details if needed.
- Make any requested updates in your branch, then push them to update the PR.

## Merge

- Once the review is approved, a maintainer will merge your changes into the main branch.
- Congratulations! You’ve contributed to the Survival Game Project.

## Project Boards and Issues

- Keep an eye on the project’s Issue Tracker and Project Boards for upcoming tasks or known bugs.
- Feel free to open a new issue if you spot a bug or want to suggest an enhancement.

## Additional Guidelines

- Test your changes thoroughly before submitting a Pull Request.
- Use descriptive filenames and folder structures that match existing conventions.
- Document any new classes or functions to help maintain readability.

By following these steps, you’ll ensure a smooth contribution process for yourself and the entire community. We appreciate your interest and look forward to collaborating with you!


## License

This project is licensed under the [MIT License](LICENSE).

## Contact

- **Lead Developer**: [halitbilici00@gmail.com](mailto:halitbilici00@gmail.com)  
- **GitHub**: [hldblc](https://github.com/hldblc)  
- **Website**: >> Under Development <<
- **Discord**: >> Under Development <<
